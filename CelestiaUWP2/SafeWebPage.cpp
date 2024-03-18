//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "SafeWebPage.h"
#if __has_include("SafeWebPage.g.cpp")
#include "SafeWebPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

namespace winrt::CelestiaUWP2::implementation
{
    SafeWebPage::SafeWebPage()
    {
    }

    void SafeWebPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        using namespace Microsoft::Web::WebView2::Core;
        auto parameter = e.Parameter().as<CelestiaUWP2::CommonWebParameter>();
        hstring webViewVersion = L"";
        try
        {
            webViewVersion = CoreWebView2Environment::GetAvailableBrowserVersionString();
        }
        catch (hresult_error const&) {}
        if (webViewVersion.empty())
        {
            WebContent().Visibility(Windows::UI::Xaml::Visibility::Visible);
            WebContent().Navigate(xaml_typename<CelestiaUWP2::LegacyWebPage>(), parameter);
        }
        else
        {
            WebContent().Visibility(Windows::UI::Xaml::Visibility::Visible);
            WebContent().Navigate(xaml_typename<CelestiaUWP2::CommonWebPage>(), parameter);
        }
    }
}
