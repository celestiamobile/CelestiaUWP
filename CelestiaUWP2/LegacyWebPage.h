//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "LegacyWebPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct LegacyWebPage : LegacyWebPageT<LegacyWebPage>
    {
        LegacyWebPage();

        fire_and_forget WebView_NavigationStarting(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::WebViewNavigationStartingEventArgs const&);
        void WebView_ScriptNotify(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::NotifyEventArgs const&);
        void WebView_DOMContentLoaded(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::WebViewDOMContentLoadedEventArgs const&);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

    private:
        Windows::Foundation::Uri initialUri;
        Windows::Foundation::Collections::IVector<hstring> matchingQueryKeys;
        CelestiaAppComponent::JavascriptBridge bridge{ nullptr };
        bool webViewOpened{ false };

        bool IsURIAllowed(Windows::Foundation::Uri const& uri);
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct LegacyWebPage : LegacyWebPageT<LegacyWebPage, implementation::LegacyWebPage>
    {
    };
}