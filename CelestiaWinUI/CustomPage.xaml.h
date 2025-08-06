// CustomPage.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "CustomPage.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct CustomPage : CustomPageT<CustomPage>
    {
        CustomPage();

        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const&);
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct CustomPage : CustomPageT<CustomPage, implementation::CustomPage>
    {
    };
}
