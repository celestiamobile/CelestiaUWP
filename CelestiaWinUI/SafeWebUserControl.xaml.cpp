// SafeWebUserControl.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "SafeWebUserControl.xaml.h"
#if __has_include("SafeWebUserControl.g.cpp")
#include "SafeWebUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Microsoft::UI::Xaml;

namespace winrt::CelestiaWinUI::implementation
{
    SafeWebUserControl::SafeWebUserControl(CelestiaWinUI::CommonWebParameter const& args) : args(args)
    {
    }

    void SafeWebUserControl::InitializeComponent()
    {
        SafeWebUserControlT::InitializeComponent();
        WebViewNotFoundView().Text(LocalizationHelper::Localize(L"WebView is not available.", L"WebView component is missing or disabled"));

        hstring webViewVersion = L"";
        try
        {
            webViewVersion = Microsoft::Web::WebView2::Core::CoreWebView2Environment::GetAvailableBrowserVersionString();
        }
        catch(hresult_error const&) {}
        if (webViewVersion.empty())
        {
            WebViewNotFoundView().Visibility(Visibility::Visible);
        }
        else
        {
            WebContent().Visibility(Visibility::Visible);
            webPage = CelestiaWinUI::CommonWebUserControl(args);
            WebContent().Children().Append(webPage);
            WebContent().SetAlignTopWithPanel(webPage, true);
            WebContent().SetAlignBottomWithPanel(webPage, true);
            WebContent().SetAlignLeftWithPanel(webPage, true);
            WebContent().SetAlignRightWithPanel(webPage, true);
        }
    }

    void SafeWebUserControl::CloseWebViewIfNeeded()
    {
        if (webPage == nullptr) return;
        webPage.CloseWebViewIfNeeded();
    }
}
