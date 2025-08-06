// CommonWebUserControl.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "CommonWebParameter.g.h"
#include "CommonWebUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct CommonWebParameter : CommonWebParameterT<CommonWebParameter>
    {
        CommonWebParameter(Windows::Foundation::Uri const& initialUri, Windows::Foundation::Collections::IVector<hstring> const& matchingQueryKeys, CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, hstring const& contextDirectory, CelestiaAppComponent::JavascriptMessageACKCallback const& ackCallback, CelestiaWinUI::WebWindowProvider const& windowProvider, bool updateWindowTitle = false);

        Windows::Foundation::Uri InitialUri();
        Windows::Foundation::Collections::IVector<hstring> MatchingQueryKeys();
        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
        hstring ContextDirectory();
        CelestiaAppComponent::JavascriptMessageACKCallback ACKCallback();
        CelestiaWinUI::WebWindowProvider WindowProvider();
        bool UpdateWindowTitle();

    private:
        Windows::Foundation::Uri initialUri;
        Windows::Foundation::Collections::IVector<hstring> matchingQueryKeys;
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        hstring contextDirectory;
        CelestiaAppComponent::JavascriptMessageACKCallback ackCallback;
        CelestiaWinUI::WebWindowProvider windowProvider;
        bool updateWindowTitle;
    };

    struct CommonWebUserControl : CommonWebUserControlT<CommonWebUserControl>
    {
        CommonWebUserControl(CelestiaWinUI::CommonWebParameter const& args);
        void InitializeComponent();

        fire_and_forget WebView_NavigationStarting(Windows::Foundation::IInspectable const&, Microsoft::Web::WebView2::Core::CoreWebView2NavigationStartingEventArgs const&);
        void WebView_WebMessageReceived(Windows::Foundation::IInspectable const&, Microsoft::Web::WebView2::Core::CoreWebView2WebMessageReceivedEventArgs const&);
        void CoreWebView2_DOMContentLoaded(Windows::Foundation::IInspectable const&, Microsoft::Web::WebView2::Core::CoreWebView2DOMContentLoadedEventArgs const&);
        void CoreWebView2_DocumentTitleChanged(Windows::Foundation::IInspectable const&, Windows::Foundation::IInspectable const&);

        void CloseWebViewIfNeeded();
        virtual ~CommonWebUserControl();

    private:
        CelestiaWinUI::CommonWebParameter args;
        CelestiaAppComponent::JavascriptBridge bridge{ nullptr };
        bool webViewOpened{ false };

        fire_and_forget EnsureWebView2();
        bool IsURIAllowed(Windows::Foundation::Uri const& uri);
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct CommonWebParameter : CommonWebParameterT<CommonWebParameter, implementation::CommonWebParameter>
    {
    };

    struct CommonWebUserControl : CommonWebUserControlT<CommonWebUserControl, implementation::CommonWebUserControl>
    {
    };
}
