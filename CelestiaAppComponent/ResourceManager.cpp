// ResourceManager.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "ResourceManager.h"
#if __has_include("ResourceManager.g.cpp")
#include "ResourceManager.g.cpp"
#endif

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <zip.h>

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Web::Http;

namespace winrt::CelestiaAppComponent::implementation
{
    bool CompareResourceItems(const CelestiaAppComponent::ResourceItem& lhs, const CelestiaAppComponent::ResourceItem& rhs)
    {
        return CompareStringEx(nullptr, SORT_DIGITSASNUMBERS, lhs.Name().c_str(), lhs.Name().size(), rhs.Name().c_str(), rhs.Name().size(), nullptr, nullptr, 0) == CSTR_LESS_THAN;
    }

    ResourceManager::ResourceManager(StorageFolder const& addonFolder, StorageFolder const& scriptFolder) : addonFolder(addonFolder), scriptFolder(scriptFolder)
    {
    }

    IAsyncAction WriteDescriptionFile(StorageFolder folder, CelestiaAppComponent::ResourceItem item)
    {
        auto file = co_await folder.CreateFileAsync(L"description.json", CreationCollisionOption::ReplaceExisting);
        co_await FileIO::WriteTextAsync(file, item.JSONRepresentation().Stringify());
    }

    template<typename Async, typename Token>
    std::decay_t<Async> MakeCancellable(Async&& async, Token&& token)
    {
        token.callback([async] { async.Cancel(); });
        return std::forward<Async>(async);
    }

    IAsyncOperation<CelestiaAppComponent::ResourceManagerDownloadFailureArgs> Unzip(CelestiaAppComponent::ResourceItem const item, hstring source, hstring destinationPath)
    {
        winrt::apartment_context ui_thread;
        co_await resume_background();
        auto cancellation = co_await get_cancellation_token();
        auto archive = zip_open(to_string(source).c_str(), 0, nullptr);
        std::string destinationFolder = to_string(destinationPath);

        if (!archive)
        {
            co_await ui_thread;
            co_return make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::Zip);
        }

        CelestiaAppComponent::ResourceManagerDownloadFailureArgs failureArgs = nullptr;
        zip_file_t *currentEntry = nullptr;
        bool hasZipError = false;
        bool hasSystemError = false;
        bool isCancelled = false;

        for (zip_int64_t i = 0; i < zip_get_num_entries(archive, 0); i += 1)
        {
            // Check cancellation
            if (cancellation())
            {
                isCancelled = true;
                break;
            }

            struct zip_stat st;
            if (zip_stat_index(archive, i, 0, &st) != 0)
            {
                hasZipError = true;
                failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::Zip);
                break;
            }

            std::filesystem::path name{ st.name };
            std::vector<std::string> components;

            for (auto const& component : name)
                components.push_back(component.string());

            bool isRegularFile = name.has_filename();
            std::filesystem::path currentDirectory = destinationFolder;

            for (int j = 0; j < components.size(); ++j)
            {
                const auto& component = components[j];
                if (component.empty())
                    continue;

                // The last one is the filename for file entries, we don't create directory for it
                if (!isRegularFile || j != components.size() - 1)
                {
                    currentDirectory = currentDirectory / component;
                    std::error_code ec;
                    bool exists = std::filesystem::exists(currentDirectory, ec);
                    if (ec)
                    {
                        hasSystemError = true;
                        failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::CreateDirectory, to_hstring(currentDirectory.string()));
                        break;
                    }

                    if (!exists)
                    {
                        bool success = std::filesystem::create_directory(currentDirectory, ec);
                        if (!success || ec)
                        {
                            hasSystemError = true;
                            failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::CreateDirectory, to_hstring(currentDirectory.string()));
                            break;
                        }
                    }
                }
            }

            if (hasSystemError)
                break;

            if (!isRegularFile)
                continue;

            currentEntry = zip_fopen_index(archive, i, 0);
            if (!currentEntry)
            {
                hasZipError = true;
                failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::Zip);
                break;
            }

            std::filesystem::path filePath = currentDirectory / name.filename();
            std::ofstream file(filePath, std::ios::binary);
            if (!file.good())
            {
                hasSystemError = true;
                failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::OpenFile, to_hstring(filePath.string()));
                break;
            }

            zip_uint64_t bytesWritten = 0;
            const uint32_t bufferSize = 4096;
            char buffer[bufferSize];
            while (bytesWritten != st.size)
            {
                // Check cancellation
                if (cancellation())
                {
                    isCancelled = true;
                    break;
                }

                auto bytesRead = zip_fread(currentEntry, buffer, bufferSize);
                if (bytesRead < 0)
                {
                    hasZipError = true;
                    failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::Zip);
                    break;
                }

                if (!file.write(buffer, bytesRead).good())
                {
                    hasSystemError = true;
                    failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::WriteFile, to_hstring(filePath.string()));
                    break;
                }

                bytesWritten += bytesRead;
            }

            zip_fclose(currentEntry);
            currentEntry = nullptr;

            if (hasSystemError || hasZipError || isCancelled)
                break;
        }

        // Clean up
        if (currentEntry != nullptr)
            zip_fclose(currentEntry);
        zip_close(archive);

        co_await ui_thread;

        // Rethrow the cancellation
        if (isCancelled)
            throw hresult_canceled();

        co_return failureArgs;
    }

    void ResourceManager::Download(CelestiaAppComponent::ResourceItem const& item)
    {
        if (tasks.find(item.ID()) != tasks.end())
            return;
        tasks[item.ID()] = DownloadAsync(item);
    }

    IAsyncAction ResourceManager::DownloadAsync(CelestiaAppComponent::ResourceItem const item)
    {
        CelestiaAppComponent::ResourceManagerDownloadFailureArgs failureArgs = nullptr;
        StorageFile tempFile = nullptr;
        auto cancellation = co_await get_cancellation_token();

        try
        {
            HttpClient client;
            auto tempFolder{ ApplicationData::Current().TemporaryFolder() };
            tempFile = co_await tempFolder.CreateFileAsync(to_hstring(GuidHelper::CreateNewGuid()) + L".zip");

            auto response = co_await client.GetAsync(Uri(item.URL()), HttpCompletionOption::ResponseHeadersRead);
            response.EnsureSuccessStatusCode();

            int64_t total = response.Content().Headers().ContentLength() != nullptr ? response.Content().Headers().ContentLength().Value() : -1L;
            bool canReportProgress = total > 0;
            auto httpStream = co_await response.Content().ReadAsInputStreamAsync();
            auto fileStream = co_await tempFile.OpenAsync(FileAccessMode::ReadWrite);
            auto fileOutputStream = fileStream.GetOutputStreamAt(0);

            int64_t totalRead = 0L;
            bool isMoreToRead = true;

            const size_t bufferSize = 4096;
            using namespace Streams;

            do
            {
                if (cancellation())
                    throw hresult_canceled();
                auto buffer{ co_await httpStream.ReadAsync(Buffer{ bufferSize }, bufferSize, InputStreamOptions::None) };
                if (buffer.Length() > 0)
                {
                    co_await fileOutputStream.WriteAsync(buffer);
                    totalRead += buffer.Length();
                    if (canReportProgress)
                    {
                        downloadProgressUpdateEvent(*this, make<ResourceManagerDownloadProgressArgs>(item, (double)totalRead / total));
                    }
                }
                else
                {
                    isMoreToRead = false;
                }
            } while (isMoreToRead);
        }
        catch (hresult_canceled const&)
        {
            failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::Cancelled);
        }
        catch (hresult_error const&)
        {
            failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::Download);
        }

        if (failureArgs != nullptr)
        {
            tasks.erase(item.ID());
            downloadFailureEvent(*this, failureArgs);
            co_return;
        }

        // Create directory
        StorageFolder destinationFolder = nullptr;
        try
        {
            destinationFolder = co_await (item.Type() == L"script" ? scriptFolder : addonFolder).CreateFolderAsync(item.ID(), CreationCollisionOption::OpenIfExists);
        }
        catch (hresult_error const&)
        {
            failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::CreateDirectory, ItemPath(item));
        }

        if (failureArgs != nullptr)
        {
            tasks.erase(item.ID());
            downloadFailureEvent(*this, failureArgs);
            co_return;
        }

        // Extract the archive
        try
        {
            failureArgs = co_await MakeCancellable(Unzip(item, tempFile.Path(), destinationFolder.Path()), cancellation);
        }
        catch (hresult_canceled const&)
        {
            failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::Cancelled);
        }
        catch (hresult_error const&)
        {
            failureArgs = make<ResourceManagerDownloadFailureArgs>(item, CelestiaAppComponent::ResourceErrorType::Zip);
        }

        if (failureArgs != nullptr)
        {
            tasks.erase(item.ID());
            downloadFailureEvent(*this, failureArgs);
            co_return;
        }

        // Create description file, ignore error
        try
        {
            co_await WriteDescriptionFile(destinationFolder, item);
        }
        catch (hresult_error const&) {}

        tasks.erase(item.ID());
        downloadSuccessEvent(*this, make<ResourceManagerDownloadSuccessArgs>(item));
    }

    hstring ResourceManager::ItemPath(CelestiaAppComponent::ResourceItem const& item)
    {
        if (item.Type() == L"script")
            return PathHelper::Combine(scriptFolder.Path(), item.ID());
        return PathHelper::Combine(addonFolder.Path(), item.ID());
    }

    hstring ResourceManager::ScriptPath(CelestiaAppComponent::ResourceItem const& item)
    {
        if (item.Type() != L"script")
            return L"";
        auto scriptFileName = item.MainScriptName();
        if (scriptFileName.empty())
            return L"";
        return PathHelper::Combine(ItemPath(item), scriptFileName);
    }

    bool DirectoryExists(hstring const& path)
    {
        auto attribs = GetFileAttributesW(path.c_str());
        return attribs != INVALID_FILE_ATTRIBUTES && ((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0);
    }

    CelestiaAppComponent::ResourceItemState ResourceManager::StateForItem(CelestiaAppComponent::ResourceItem const& item)
    {
        if (tasks.find(item.ID()) != tasks.end())
            return ResourceItemState::Downloading;
        if (DirectoryExists(ItemPath(item)))
            return ResourceItemState::Installed;
        return CelestiaAppComponent::ResourceItemState::None;
    }

    void ResourceManager::Cancel(CelestiaAppComponent::ResourceItem const& item)
    {
        auto task = tasks.find(item.ID());
        if (task != tasks.end())
            task->second.Cancel();
    }

    IAsyncAction ResourceManager::Uninstall(CelestiaAppComponent::ResourceItem const item)
    {
        try
        {
            auto folder = co_await StorageFolder::GetFolderFromPathAsync(ItemPath(item));
            co_await folder.DeleteAsync();
        }
        catch (hresult_error const&) {}
    }

    IAsyncOperation<Collections::IVector<CelestiaAppComponent::ResourceItem>> ResourceManager::InstalledItems()
    {
        std::vector<CelestiaAppComponent::ResourceItem> items;
        try
        {
            for (const auto& folder : co_await scriptFolder.GetFoldersAsync())
            {
                try
                {
                    auto descriptionFile = co_await folder.GetFileAsync(L"description.json");
                    auto fileContent = co_await FileIO::ReadTextAsync(descriptionFile);
                    auto parsedItem = CelestiaAppComponent::ResourceItem::TryParse(fileContent);
                    if (parsedItem != nullptr && parsedItem.ID() == folder.Name() && parsedItem.Type() == L"script")
                    {
                        items.push_back(parsedItem);
                    }
                }
                catch (hresult_error const&) {}
            }
        }
        catch (hresult_error const&) {}
        try
        {
            for (const auto& folder : co_await addonFolder.GetFoldersAsync())
            {
                try
                {
                    auto descriptionFile = co_await folder.GetFileAsync(L"description.json");
                    auto fileContent = co_await FileIO::ReadTextAsync(descriptionFile);
                    auto parsedItem = CelestiaAppComponent::ResourceItem::TryParse(fileContent);
                    if (parsedItem != nullptr && parsedItem.ID() == folder.Name() && parsedItem.Type() != L"script")
                    {
                        items.push_back(parsedItem);
                    }
                }
                catch (hresult_error const&) {}
            }
        }
        catch (hresult_error const&) {}
        std::sort(items.begin(), items.end(), CompareResourceItems);
        
        co_return single_threaded_vector<CelestiaAppComponent::ResourceItem>({ items.begin(), items.end() });
    }

    IAsyncOperation<CelestiaAppComponent::ResourceItem> ResourceManager::InstalledItem(CelestiaAppComponent::ResourceItem const item)
    {
        CelestiaAppComponent::ResourceItem itemOnDisk = nullptr;
        try
        {
            auto folder = co_await StorageFolder::GetFolderFromPathAsync(ItemPath(item));
            auto descriptionFile = co_await folder.GetFileAsync(L"description.json");
            auto fileContent = co_await FileIO::ReadTextAsync(descriptionFile);
            itemOnDisk = CelestiaAppComponent::ResourceItem::TryParse(fileContent);
        }
        catch (hresult_error const&) {}
        co_return itemOnDisk;
    }

    event_token ResourceManager::DownloadProgressUpdate(EventHandler<CelestiaAppComponent::ResourceManagerDownloadProgressArgs> const& handler)
    {
        return downloadProgressUpdateEvent.add(handler);
    }

    void ResourceManager::DownloadProgressUpdate(event_token const& token)
    {
        downloadProgressUpdateEvent.remove(token);
    }

    event_token ResourceManager::DownloadSuccess(EventHandler<CelestiaAppComponent::ResourceManagerDownloadSuccessArgs> const& handler)
    {
        return downloadSuccessEvent.add(handler);
    }

    void ResourceManager::DownloadSuccess(event_token const& token)
    {
        downloadSuccessEvent.remove(token);
    }

    event_token ResourceManager::DownloadFailure(EventHandler<CelestiaAppComponent::ResourceManagerDownloadFailureArgs> const& handler)
    {
        return downloadFailureEvent.add(handler);
    }

    void ResourceManager::DownloadFailure(event_token const& token)
    {
        downloadFailureEvent.remove(token);
    }
}
