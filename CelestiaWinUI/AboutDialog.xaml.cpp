// AboutDialog.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "AboutDialog.xaml.h"
#if __has_include("AboutDialog.g.cpp")
#include "AboutDialog.g.cpp"
#endif

#include <fmt/format.h>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    AboutDialog::AboutDialog()
    {
    }

    void AboutDialog::InitializeComponent()
    {
        AboutDialogT::InitializeComponent();

        Title(box_value(LocalizationHelper::Localize(L"About", L"About Celestia")));
        PrimaryButtonText(LocalizationHelper::Localize(L"OK", L""));

        Windows::Web::Http::HttpFormUrlEncodedContent query({ { L"lang", LocalizationHelper::Locale()} });
        hstring url = hstring(L"https://celestia.mobi") + L"?" + query.ToString();
        LinkButton().NavigateUri(Uri(url));

        auto version = SystemInformation::Instance().ApplicationVersion();
        VersionLabel().Text(to_hstring(fmt::format("Celestia {0}.{1}.{2}.{3}", version.Major, version.Minor, version.Build, version.Revision)));
    }
}
