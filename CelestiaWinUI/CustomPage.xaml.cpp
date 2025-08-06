// CustomPage.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CustomPage.xaml.h"
#if __has_include("CustomPage.g.cpp")
#include "CustomPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::CelestiaWinUI::implementation
{
    CustomPage::CustomPage()
    {
    }

    void CustomPage::OnNavigatedTo(Navigation::NavigationEventArgs const& args)
    {
        auto customView = args.Parameter().try_as<UIElement>();
        if (customView != nullptr)
        {
            Content(customView);
        }
    }
}
