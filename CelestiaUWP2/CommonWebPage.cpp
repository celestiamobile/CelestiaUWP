//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CommonWebPage.h"
#if __has_include("CommonWebPage.g.cpp")
#include "CommonWebPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Microsoft::Web::WebView2::Core;

namespace winrt::CelestiaUWP2::implementation
{
    CommonWebPage::CommonWebPage() : initialUri(nullptr), matchingQueryKeys(nullptr)
    {
    }

    void CommonWebPage::CloseWebViewIfNeeded()
    {
        if (webViewOpened)
        {
            WebView().Close();
            webViewOpened = false;
        }
    }

    CommonWebPage::~CommonWebPage()
    {
        CloseWebViewIfNeeded();
    }

    fire_and_forget CommonWebPage::WebView_NavigationStarting(IInspectable const&, CoreWebView2NavigationStartingEventArgs const& args)
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

    void CommonWebPage::WebView_WebMessageReceived(IInspectable const&, CoreWebView2WebMessageReceivedEventArgs const& args)
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

    void CommonWebPage::CoreWebView2_DOMContentLoaded(IInspectable const&, CoreWebView2DOMContentLoadedEventArgs const&)
    {
        LoadingIndicator().Visibility(Visibility::Collapsed);
    }

    fire_and_forget CommonWebPage::EnsureWebView2()
    {
        try
        {
            co_await WebView().EnsureCoreWebView2Async();
        }
        catch (hresult_error const&)
        {
            co_return;
        }
        webViewOpened = true;
        WebView().Source(initialUri);
        WebView().CoreWebView2().DOMContentLoaded({ get_weak(), &CommonWebPage::CoreWebView2_DOMContentLoaded });
    }

    bool CommonWebPage::IsURIAllowed(Uri const& uri)
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

    void CommonWebPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto parameter = e.Parameter().as<CelestiaUWP2::CommonWebParameter>();
        initialUri = parameter.InitialUri();
        matchingQueryKeys = parameter.MatchingQueryKeys();
        bridge = CelestiaAppComponent::JavascriptBridge(parameter.AppCore(), parameter.Renderer(), parameter.ContextDirectory(), parameter.ACKCallback(), [weak_this{ get_weak() }](hstring const title, Uri const uri)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [title, uri]()
                    {
                        using namespace Windows::ApplicationModel::DataTransfer;
                        auto transferManager = DataTransferManager::GetForCurrentView();
                        transferManager.DataRequested([title, uri](IInspectable const&, DataRequestedEventArgs const& args)
                            {
                                args.Request().Data().Properties().Title(title);
                                args.Request().Data().SetWebLink(uri);
                            });
                        DataTransferManager::ShowShareUI();
                    });
            });
        EnsureWebView2();
    }
}
