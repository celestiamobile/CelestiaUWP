// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

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
