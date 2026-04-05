// InfoUserControl.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "InfoUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct InfoUserControl : InfoUserControlT<InfoUserControl>
    {
        InfoUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaComponent::CelestiaSelection const& selection, bool preserveMargin = true);
        void InitializeComponent();

        event_token ShowSubsystem(Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs> const& handler);
        void ShowSubsystem(event_token const& token) noexcept;
        event_token GetInfo(Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs> const& handler);
        void GetInfo(event_token const& token) noexcept;
        event_token OpenURL(Windows::Foundation::EventHandler<hstring> const& handler);
        void OpenURL(event_token const& token) noexcept;

        void CockpitCheckbox_Checked(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void CockpitCheckbox_Unchecked(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void ControlStrip_ShowSubsystem(Windows::Foundation::IInspectable const&, CelestiaWinUI::InfoShowSubsystemArgs const&);
        void ControlStrip_GetInfo(Windows::Foundation::IInspectable const&, CelestiaWinUI::InfoGetInfoArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaComponent::CelestiaSelection selection;
        bool preserveMargin;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserAction> actions;
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs>> showSubsystemEvent;
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs>> getInfoEvent;
        event<Windows::Foundation::EventHandler<hstring>> openURLEvent;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct InfoUserControl : InfoUserControlT<InfoUserControl, implementation::InfoUserControl>
    {
    };
}
