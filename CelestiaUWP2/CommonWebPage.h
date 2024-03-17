//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "CommonWebPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct CommonWebPage : CommonWebPageT<CommonWebPage>
    {
        CommonWebPage();

        fire_and_forget WebView_NavigationStarting(Windows::Foundation::IInspectable const&, Microsoft::Web::WebView2::Core::CoreWebView2NavigationStartingEventArgs const&);
        void WebView_WebMessageReceived(Windows::Foundation::IInspectable const&, Microsoft::Web::WebView2::Core::CoreWebView2WebMessageReceivedEventArgs const&);
        void CoreWebView2_DOMContentLoaded(Windows::Foundation::IInspectable const&, Microsoft::Web::WebView2::Core::CoreWebView2DOMContentLoadedEventArgs const&);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

        void CloseWebViewIfNeeded();
        virtual ~CommonWebPage();

    private:
        Windows::Foundation::Uri initialUri;
        Windows::Foundation::Collections::IVector<hstring> matchingQueryKeys;
        CelestiaAppComponent::JavascriptBridge bridge{ nullptr };
        bool webViewOpened{ false };

        fire_and_forget EnsureWebView2();
        bool IsURIAllowed(Windows::Foundation::Uri const& uri);
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct CommonWebPage : CommonWebPageT<CommonWebPage, implementation::CommonWebPage>
    {
    };
}
