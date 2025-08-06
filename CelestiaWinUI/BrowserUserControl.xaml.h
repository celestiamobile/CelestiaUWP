// BrowserUserControl.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.


#pragma once

#include "BrowserUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct BrowserUserControl : BrowserUserControlT<BrowserUserControl>
    {
        BrowserUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        void InitializeComponent();

        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItemTab> RootItems();

        void NavigationView_SelectionChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItemTab> rootItems;

        void LoadData();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct BrowserUserControl : BrowserUserControlT<BrowserUserControl, implementation::BrowserUserControl>
    {
    };
}
