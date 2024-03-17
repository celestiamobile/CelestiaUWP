//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "SafeWebPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct SafeWebPage : SafeWebPageT<SafeWebPage>
    {
        SafeWebPage();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct SafeWebPage : SafeWebPageT<SafeWebPage, implementation::SafeWebPage>
    {
    };
}