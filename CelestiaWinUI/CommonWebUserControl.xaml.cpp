#include "pch.h"
#include "CommonWebUserControl.xaml.h"
#if __has_include("CommonWebUserControlArgs.g.cpp")
#include "CommonWebUserControlArgs.g.cpp"
#endif
#if __has_include("CommonWebUserControl.g.cpp")
#include "CommonWebUserControl.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::Web::WebView2::Core;

namespace winrt::CelestiaWinUI::implementation
{
    CommonWebUserControlArgs::CommonWebUserControlArgs(Uri const& initialUri, Collections::IVector<hstring> const& matchingQueryKeys, CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, hstring const& contextDirectory, CelestiaAppComponent::JavascriptMessageACKCallback const& ackCallback, CelestiaWinUI::WebWindowProvider const& windowProvider)
        : initialUri(initialUri), matchingQueryKeys(matchingQueryKeys), appCore(appCore), renderer(renderer), contextDirectory(contextDirectory), ackCallback(ackCallback), windowProvider(windowProvider)
    {
    }

    Uri CommonWebUserControlArgs::InitialUri()
    {
        return initialUri;
    }

    Collections::IVector<hstring> CommonWebUserControlArgs::MatchingQueryKeys()
    {
        return matchingQueryKeys;
    }

    CelestiaComponent::CelestiaAppCore CommonWebUserControlArgs::AppCore() {
        return appCore;
    }

    CelestiaComponent::CelestiaRenderer CommonWebUserControlArgs::Renderer()
    {
        return renderer;
    }

    hstring CommonWebUserControlArgs::ContextDirectory()
    {
        return contextDirectory;
    }

    CelestiaAppComponent::JavascriptMessageACKCallback CommonWebUserControlArgs::ACKCallback()
    {
        return ackCallback;
    };

    CelestiaWinUI::WebWindowProvider CommonWebUserControlArgs::WindowProvider()
    {
        return windowProvider;
    }

    CommonWebUserControl::CommonWebUserControl(CelestiaWinUI::CommonWebUserControlArgs const& args) : initialUri(args.InitialUri()), matchingQueryKeys(args.MatchingQueryKeys()), windowProvider(args.WindowProvider())
    {
        InitializeComponent();

        bridge = CelestiaAppComponent::JavascriptBridge(args.AppCore(), args.Renderer(), args.ContextDirectory(), args.ACKCallback(), [weak_this{ get_weak() }](hstring const& title, Uri const& uri)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this != nullptr)
                {
                    auto windowProvider = strong_this->windowProvider;
                    strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [windowProvider, title, uri]()
                        {
                            auto window = windowProvider != nullptr ? windowProvider() : nullptr;
                            if (window == nullptr) return;
                                WindowHelper::ShowShareLink(window, title, uri);
                        });
                }
            });

        EnsureWebView2();
    }

    void CommonWebUserControl::CloseWebViewIfNeeded()
    {
        if (webViewOpened)
        {
            WebView().Close();
            webViewOpened = false;
        }
    }

    CommonWebUserControl::~CommonWebUserControl()
    {
        CloseWebViewIfNeeded();
    }

    fire_and_forget CommonWebUserControl::WebView_NavigationStarting(IInspectable const&, CoreWebView2NavigationStartingEventArgs const& args)
    {
        if (args.Uri().empty())
        {
            args.Cancel(true);
            return;
        }
        Uri uri{ args.Uri() };
        if (!IsURIAllowed(uri))
        {
            args.Cancel(true);
            co_await Windows::System::Launcher::LaunchUriAsync(uri);
        }
    }

    void CommonWebUserControl::WebView_WebMessageReceived(IInspectable const&, CoreWebView2WebMessageReceivedEventArgs const& args)
    {
        try
        {
            auto message = args.TryGetWebMessageAsString();
            if (!message.empty() && bridge != nullptr)
            {
                bridge.ReceivedMessage(message);
            }
        }
        catch (hresult_error const&) {}
    }

    void CommonWebUserControl::CoreWebView2_DOMContentLoaded(IInspectable const&, CoreWebView2DOMContentLoadedEventArgs const&)
    {
        LoadingIndicator().Visibility(Visibility::Collapsed);
    }

    fire_and_forget CommonWebUserControl::EnsureWebView2()
    {
        try
        {
            co_await WebView().EnsureCoreWebView2Async();
        }
        catch (hresult_error const&)
        {
            return;
        }
        webViewOpened = true;
        WebView().Source(initialUri);
        windowProvider().Closed([weak_this{ get_weak() }](IInspectable const&, WindowEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->CloseWebViewIfNeeded();
            });
        WebView().CoreWebView2().DOMContentLoaded({ get_weak(), &CommonWebUserControl::CoreWebView2_DOMContentLoaded });
    }

    bool CommonWebUserControl::IsURIAllowed(Uri const& uri)
    {
        if (uri.Host() != initialUri.Host() || uri.Path() != initialUri.Path())
            return false;

        auto query = uri.QueryParsed();
        auto initialQuery = initialUri.QueryParsed();

        for (const auto& key : matchingQueryKeys)
        {
            if (query.GetFirstValueByName(key) != initialQuery.GetFirstValueByName(key))
                return false;
        }
        return true;
    }
}
