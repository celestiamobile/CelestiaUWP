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

        event_token OpenURL(Windows::Foundation::EventHandler<hstring> const& handler);
        void OpenURL(event_token const& token) noexcept;
        event_token GetInfo(Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs> const& handler);
        void GetInfo(event_token const& token) noexcept;
        event_token ShowSubsystem(Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs> const& handler);
        void ShowSubsystem(event_token const& token) noexcept;

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItemTab> rootItems;

        void LoadData();
        event<Windows::Foundation::EventHandler<hstring>> openURLEvent;
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs>> getInfoEvent;
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs>> showSubsystemEvent;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct BrowserUserControl : BrowserUserControlT<BrowserUserControl, implementation::BrowserUserControl>
    {
    };
}
