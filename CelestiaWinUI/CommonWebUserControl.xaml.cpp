#include "pch.h"
#include "CommonWebUserControl.xaml.h"
#if __has_include("CommonWebParameter.g.cpp")
#include "CommonWebParameter.g.cpp"
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
    CommonWebParameter::CommonWebParameter(Uri const& initialUri, Collections::IVector<hstring> const& matchingQueryKeys, CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, hstring const& contextDirectory, CelestiaAppComponent::JavascriptMessageACKCallback const& ackCallback, CelestiaWinUI::WebWindowProvider const& windowProvider)
        : initialUri(initialUri), matchingQueryKeys(matchingQueryKeys), appCore(appCore), renderer(renderer), contextDirectory(contextDirectory), ackCallback(ackCallback), windowProvider(windowProvider)
    {
    }

    Uri CommonWebParameter::InitialUri()
    {
        return initialUri;
    }

    Collections::IVector<hstring> CommonWebParameter::MatchingQueryKeys()
    {
        return matchingQueryKeys;
    }

    CelestiaComponent::CelestiaAppCore CommonWebParameter::AppCore() {
        return appCore;
    }

    CelestiaComponent::CelestiaRenderer CommonWebParameter::Renderer()
    {
        return renderer;
    }

    hstring CommonWebParameter::ContextDirectory()
    {
        return contextDirectory;
    }

    CelestiaAppComponent::JavascriptMessageACKCallback CommonWebParameter::ACKCallback()
    {
        return ackCallback;
    };

    CelestiaWinUI::WebWindowProvider CommonWebParameter::WindowProvider()
    {
        return windowProvider;
    }

    CommonWebUserControl::CommonWebUserControl(CelestiaWinUI::CommonWebParameter const& args) : args(args)
    {
    }

    void CommonWebUserControl::InitializeComponent()
    {
        CommonWebUserControlT::InitializeComponent();
        webViewOpened = true;
        bridge = CelestiaAppComponent::JavascriptBridge(args.AppCore(), args.Renderer(), args.ContextDirectory(), args.ACKCallback(), [weak_this{ get_weak() }](hstring const& title, Uri const& uri)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this != nullptr)
                {
                    auto windowProvider = strong_this->args.WindowProvider();
                    strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [windowProvider, title, uri]()
                        {
                            auto window = windowProvider != nullptr ? windowProvider() : nullptr;
                            if (window == nullptr) return;
                                WindowHelper::ShowShareLink(window, title, uri);
                        });
                }
            });
        args.WindowProvider()().Closed([weak_this{ get_weak() }](IInspectable const&, WindowEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->CloseWebViewIfNeeded();
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

    fire_and_forget CommonWebUserControl::WebView_NavigationStarting(IInspectable const&, CoreWebView2NavigationStartingEventArgs const& navigationArgs)
    {
        if (navigationArgs.Uri().empty())
        {
            navigationArgs.Cancel(true);
            return;
        }
        Uri uri{ navigationArgs.Uri() };
        if (!IsURIAllowed(uri))
        {
            navigationArgs.Cancel(true);
            co_await Windows::System::Launcher::LaunchUriAsync(uri);
        }
    }

    void CommonWebUserControl::WebView_WebMessageReceived(IInspectable const&, CoreWebView2WebMessageReceivedEventArgs const& messageArgs)
    {
        try
        {
            auto message = messageArgs.TryGetWebMessageAsString();
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
        auto weak_this{ get_weak() };
        try
        {
            co_await WebView().EnsureCoreWebView2Async();
        }
        catch (hresult_error const&)
        {
            co_return;
        }
        auto strong_this = weak_this.get();
        if (strong_this == nullptr)
            co_return;
        strong_this->WebView().Source(strong_this->args.InitialUri());
        strong_this->WebView().CoreWebView2().DOMContentLoaded({ strong_this->get_weak(), &CommonWebUserControl::CoreWebView2_DOMContentLoaded });
    }

    bool CommonWebUserControl::IsURIAllowed(Uri const& uri)
    {
        if (uri.Host() != args.InitialUri().Host() || uri.Path() != args.InitialUri().Path())
            return false;

        auto query = uri.QueryParsed();
        auto initialQuery = args.InitialUri().QueryParsed();

        for (const auto& key : args.MatchingQueryKeys())
        {
            if (query.GetFirstValueByName(key) != initialQuery.GetFirstValueByName(key))
                return false;
        }
        return true;
    }
}
