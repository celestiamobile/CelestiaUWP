#include "pch.h"
#include "BookmarkHelper.h"
#if __has_include("BookmarkHelper.g.cpp")
#include "BookmarkHelper.g.cpp"
#endif

#include "ObservableVector.h"

using namespace winrt;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;

namespace winrt::CelestiaAppComponent::implementation
{
    IAsyncOperation<Collections::IObservableVector<CelestiaAppComponent::BookmarkNode>> BookmarkHelper::ReadBookmarks()
    {
        auto results = single_threaded_observable_vector<CelestiaAppComponent::BookmarkNode>();
        Windows::Storage::StorageFolder localFolder = Windows::Storage::ApplicationData::Current().LocalFolder();
        try
        {
            auto bookmarkFile{ co_await localFolder.GetFileAsync(L"bookmarks.json") };
            auto bookmarkFileContent = co_await Windows::Storage::FileIO::ReadTextAsync(bookmarkFile);
            auto parsedArray = JsonArray::Parse(bookmarkFileContent);
            for (const auto& entry : parsedArray)
            {
                auto generatedNode = CelestiaAppComponent::BookmarkNode(entry.GetObject());
                if (generatedNode.HasErrors())
                    break;
                results.Append(generatedNode);
            }
        }
        catch (hresult_error const&) {};
        co_return results;
    }

    IAsyncAction BookmarkHelper::WriteBookmarks(Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> const& bookmarks)
    {
        JsonArray jsonArray;
        for (const auto& value : bookmarks)
        {
            jsonArray.Append(value.JsonRepresentation());
        }
        Windows::Storage::StorageFolder localFolder = Windows::Storage::ApplicationData::Current().LocalFolder();
        try
        {
            auto bookmarkFile{ co_await localFolder.CreateFileAsync(L"bookmarks.json", Windows::Storage::CreationCollisionOption::ReplaceExisting) };
            co_await Windows::Storage::FileIO::WriteTextAsync(bookmarkFile, jsonArray.Stringify());
        }
        catch (hresult_error const&) {};
        co_return;
    }

    Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> BookmarkHelper::CreateEmptyList()
    {
        return single_threaded_observable_vector<CelestiaAppComponent::BookmarkNode>();
    }

    Microsoft::UI::Xaml::Interop::IBindableObservableVector BookmarkHelper::ConvertToBindable(Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> const& list)
    {
        return make<ObservableVector<CelestiaAppComponent::BookmarkNode>>(list);
    }
}