#include "pch.h"
#include "ResourceItemUserControl.xaml.h"
#if __has_include("ResourceItemUserControl.g.cpp")
#include "ResourceItemUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;

namespace winrt::CelestiaWinUI::implementation
{
    ResourceItemUserControl::ResourceItemUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaAppComponent::ResourceItem const& item, ResourceManager const& resourceManager, CelestiaWinUI::WebWindowProvider const& windowProvider)
        : appCore(appCore), renderer(renderer), item(item), resourceManager(resourceManager), windowProvider(windowProvider)
    {
    }

    void ResourceItemUserControl::InitializeComponent()
    {
        ResourceItemUserControlT::InitializeComponent();

        UpdateState();
        ReloadItem();

        resourceManager.DownloadSuccess({ get_weak(), &ResourceItemUserControl::ResourceManager_DownloadSuccess });
        resourceManager.DownloadFailure({ get_weak(), &ResourceItemUserControl::ResourceManager_DownloadFailure });
        resourceManager.DownloadProgressUpdate({ get_weak(), &ResourceItemUserControl::ResourceManager_DownloadProgressUpdate });

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
        webPage = CelestiaWinUI::SafeWebUserControl(CelestiaWinUI::CommonWebParameter(Uri(uriString), single_threaded_vector<hstring>(std::vector<hstring>{ L"item" }), appCore, renderer, resourceManager.ItemPath(item), nullptr, windowProvider));
        WebContent().Children().Append(webPage);
        WebContent().SetAlignTopWithPanel(webPage, true);
        WebContent().SetAlignBottomWithPanel(webPage, true);
        WebContent().SetAlignLeftWithPanel(webPage, true);
        WebContent().SetAlignRightWithPanel(webPage, true);
    }

    void ResourceItemUserControl::CloseWebViewIfNeeded()
    {
        if (webPage != nullptr)
            webPage.CloseWebViewIfNeeded();
    }

    fire_and_forget ResourceItemUserControl::ActionButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto weak_this{ get_weak() };
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
        auto strong_this = weak_this.get();
        if (strong_this)
            strong_this->UpdateState();
    }

    bool FileExists(hstring const& path)
    {
        auto attribs = GetFileAttributesW(path.c_str());
        return attribs != INVALID_FILE_ATTRIBUTES && ((attribs & (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE)) != 0);
    }

    void ResourceItemUserControl::GoButton_Click(IInspectable const&, RoutedEventArgs const&)
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

    void ResourceItemUserControl::ResourceManager_DownloadProgressUpdate(IInspectable const&, ResourceManagerDownloadProgressArgs const& args)
    {
        if (item.ID() != args.Item().ID()) return;
        InstallProgressBar().Value(args.Progress());
    }

    void ResourceItemUserControl::ResourceManager_DownloadSuccess(IInspectable const&, ResourceManagerDownloadSuccessArgs const& args)
    {
        if (item.ID() != args.Item().ID()) return;
        UpdateState();
    }

    fire_and_forget ResourceItemUserControl::ResourceManager_DownloadFailure(IInspectable const&, ResourceManagerDownloadFailureArgs const& args)
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


    void ResourceItemUserControl::UpdateState()
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

    fire_and_forget ResourceItemUserControl::ReloadItem()
    {
        using namespace Windows::Web::Http;

        HttpClient client;
        HttpFormUrlEncodedContent query({ {L"item", item.ID()}, {L"lang", LocalizationHelper::Locale()}});
        auto url = hstring(L"https://celestia.mobi/api") + L"/resource/item" + L"?" + query.ToString();
        auto weak_this{ get_weak() };
        try
        {
            auto response = co_await client.GetAsync(Uri(url));
            response.EnsureSuccessStatusCode();
            auto content = co_await response.Content().ReadAsStringAsync();

            auto strong_this = weak_this.get();
            if (strong_this == nullptr)
                co_return;

            auto requestResult = RequestResult::TryParse(content);
            if (requestResult.Status() == 0)
            {
                auto newItem = ResourceItem::TryParse(requestResult.Info().Detail());
                strong_this->item = newItem;
                strong_this->UpdateState();
            }
        }
        catch (hresult_error const&) {}
    }
}
