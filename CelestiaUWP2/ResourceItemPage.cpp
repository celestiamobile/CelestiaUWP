//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "ResourceItemPage.h"

#if __has_include("ResourceItemParameter.g.cpp")
#include "ResourceItemParameter.g.cpp"
#endif
#if __has_include("ResourceItemPage.g.cpp")
#include "ResourceItemPage.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaComponent;
using namespace CelestiaAppComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

namespace winrt::CelestiaUWP2::implementation
{
    ResourceItemParameter::ResourceItemParameter(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, ResourceItem const& item, CelestiaAppComponent::ResourceManager const& resourceManager)
        : appCore(appCore), renderer(renderer), item(item), resourceManager(resourceManager)
    {
    }

    CelestiaAppCore ResourceItemParameter::AppCore()
    {
        return appCore;
    }

    CelestiaRenderer ResourceItemParameter::Renderer()
    {
        return renderer;
    }

    ResourceItem ResourceItemParameter::Item()
    {
        return item;
    }

    CelestiaAppComponent::ResourceManager ResourceItemParameter::ResourceManager()
    {
        return resourceManager;
    }

    ResourceItemPage::ResourceItemPage() : appCore(nullptr), renderer(nullptr), item(nullptr), resourceManager(nullptr)
    {
    }

    void ResourceItemPage::Load()
    {
        UpdateState();
        ReloadItem();

        resourceManager.DownloadSuccess({ get_weak(), &ResourceItemPage::ResourceManager_DownloadSuccess });
        resourceManager.DownloadFailure({ get_weak(), &ResourceItemPage::ResourceManager_DownloadFailure });
        resourceManager.DownloadProgressUpdate({ get_weak(), &ResourceItemPage::ResourceManager_DownloadProgressUpdate });

        bool isXbox = Windows::System::Profile::AnalyticsInfo::VersionInfo().DeviceFamily() == L"Windows.Xbox";
        auto uriString = hstring(L"https://celestia.mobi/resources/item") + L"?" + Windows::Web::Http::HttpFormUrlEncodedContent(
            {
                {L"lang", LocalizationHelper::Locale()},
                {L"item", item.ID()},
                {L"platform", isXbox ? L"xbox" : L"uwp"},
                {L"titleVisibility", L"visible"},
                {L"transparentBackground", L"1"}
            }
        ).ToString();
        WebContent().Navigate(xaml_typename<CelestiaUWP2::SafeWebPage>(), CelestiaUWP2::CommonWebParameter(Uri(uriString), single_threaded_vector<hstring>(std::vector<hstring>{ L"item" }), appCore, renderer, resourceManager.ItemPath(item), nullptr));
    }

    fire_and_forget ResourceItemPage::ActionButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        switch (resourceManager.StateForItem(item))
        {
        case ResourceItemState::Installed:
            co_await resourceManager.Uninstall(item);
            break;
        case ResourceItemState::Downloading:
            resourceManager.Cancel(item);
            break;
        case ResourceItemState::None:
            resourceManager.Download(item);
            break;
        default:
            break;
        }
        UpdateState();
    }

    bool FileExists(hstring const& path)
    {
        auto attribs = GetFileAttributesW(path.c_str());
        return attribs != INVALID_FILE_ATTRIBUTES && ((attribs & (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE)) != 0);
    }

    void ResourceItemPage::GoButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (item.Type() == L"script")
        {
            auto scriptPath = resourceManager.ScriptPath(item);
            if (!scriptPath.empty())
            {
                renderer.EnqueueTask([this, scriptPath]()
                    {
                        appCore.RunScript(scriptPath);
                    });
            }
        }
        else
        {
            auto demoObjectName = item.DemoObjectName();
            if (!demoObjectName.empty())
            {
                renderer.EnqueueTask([this, demoObjectName]()
                    {
                        auto selection = appCore.Simulation().Find(demoObjectName);
                        if (selection.IsEmpty())
                        {
                            appCore.Simulation().Selection(selection);
                            appCore.CharEnter(103);
                        }
                    });
            }
        }
    }

    void ResourceItemPage::ResourceManager_DownloadProgressUpdate(IInspectable const&, ResourceManagerDownloadProgressArgs const& args)
    {
        if (item.ID() != args.Item().ID()) return;
        InstallProgressBar().Value(args.Progress());
    }

    void ResourceItemPage::ResourceManager_DownloadSuccess(IInspectable const&, ResourceManagerDownloadSuccessArgs const& args)
    {
        if (item.ID() != args.Item().ID()) return;
        UpdateState();
    }

    fire_and_forget ResourceItemPage::ResourceManager_DownloadFailure(IInspectable const&, ResourceManagerDownloadFailureArgs const& args)
    {
        if (item.ID() != args.Item().ID()) return;
        UpdateState();

        hstring message = L"";
        switch (args.ErrorType())
        {
            case ResourceErrorType::Cancelled:
                break;
            case ResourceErrorType::Download:
                message = LocalizationHelper::Localize(L"Error downloading add-on", L"");
                break;
            case ResourceErrorType::Zip:
                message = LocalizationHelper::Localize(L"Error unzipping add-on", L"");
                break;
            case ResourceErrorType::CreateDirectory:
                message = LocalizationHelper::Localize(L"Error creating directory for add-on", L"");
                break;
            case ResourceErrorType::OpenFile:
                message = LocalizationHelper::Localize(L"Error opening file for saving add-on", L"");
                break;
            case ResourceErrorType::WriteFile:
                message = LocalizationHelper::Localize(L"Error writing data file for add-on", L"");
                break;
            default:
                break;
        }

        if (!message.empty())
            co_await ContentDialogHelper::ShowAlert(*this, LocalizationHelper::Localize(L"Failed to download or install this add-on.", L""), message);
    }


    void ResourceItemPage::UpdateState()
    {
        auto state = resourceManager.StateForItem(item);
        switch (state)
        {
        case ResourceItemState::Installed:
            InstallProgressBar().Visibility(Visibility::Collapsed);
            ActionButton().Content(box_value(LocalizationHelper::Localize(L"Uninstall", L"")));
            break;
        case ResourceItemState::Downloading:
            InstallProgressBar().Visibility(Visibility::Visible);
            ActionButton().Content(box_value(LocalizationHelper::Localize(L"Cancel", L"")));
            break;
        case ResourceItemState::None:
            InstallProgressBar().Visibility(Visibility::Collapsed);
            ActionButton().Content(box_value(LocalizationHelper::Localize(L"Install", L"")));
            break;
        default:
            break;
        }

        if (state != ResourceItemState::Installed)
        {
            GoButton().Visibility(Visibility::Collapsed);
        }
        else if (item.Type() == L"script")
        {
            GoButton().Content(box_value(LocalizationHelper::Localize(L"Run", L"")));
            auto scriptPath = resourceManager.ScriptPath(item);
            if (!scriptPath.empty())
            {
                GoButton().Visibility(FileExists(scriptPath) ? Visibility::Visible : Visibility::Collapsed);
            }
            else
            {
                GoButton().Visibility(Visibility::Collapsed);
            }
        }
        else
        {
            GoButton().Content(box_value(LocalizationHelper::Localize(L"Go", L"")));
            auto demoObjectName = item.DemoObjectName();
            GoButton().Visibility(!demoObjectName.empty() ? Visibility::Visible : Visibility::Collapsed);
        }
    }

    fire_and_forget ResourceItemPage::ReloadItem()
    {
        using namespace Windows::Web::Http;

        HttpClient client;
        HttpFormUrlEncodedContent query({ {L"item", item.ID()}, {L"lang", LocalizationHelper::Locale()}});
        auto url = hstring(L"https://celestia.mobi/api") + L"/resource/item" + L"?" + query.ToString();
        try
        {
            auto response = co_await client.GetAsync(Uri(url));
            response.EnsureSuccessStatusCode();
            auto content = co_await response.Content().ReadAsStringAsync();
            auto requestResult = RequestResult::TryParse(content);
            if (requestResult.Status() == 0)
            {
                auto newItem = ResourceItem::TryParse(requestResult.Info().Detail());
                item = newItem;
                UpdateState();
            }
        }
        catch (hresult_error const&) {}
    }

    void ResourceItemPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto parameter = e.Parameter().as<CelestiaUWP2::ResourceItemParameter>();
        appCore = parameter.AppCore();
        renderer = parameter.Renderer();
        item = parameter.Item();
        resourceManager = parameter.ResourceManager();
        Load();
    }
}
