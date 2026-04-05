// BrowserItemUserControl.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.


#pragma once

#include "BrowserItemUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct BrowserItemUserControl : BrowserItemUserControlT<BrowserItemUserControl>
    {
        BrowserItemUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaAppComponent::BrowserItemTab const& item, bool preserveMargin = true);
        void InitializeComponent();

        Microsoft::UI::Xaml::Interop::IBindableObservableVector RootItem();

        void Tree_SelectionChanged(Microsoft::UI::Xaml::Controls::TreeView const&, Microsoft::UI::Xaml::Controls::TreeViewSelectionChangedEventArgs const&);

        event_token GetInfo(Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs> const& handler);
        void GetInfo(event_token const& token) noexcept;
        event_token ShowSubsystem(Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs> const& handler);
        void ShowSubsystem(event_token const& token) noexcept;
        event_token OpenURL(Windows::Foundation::EventHandler<hstring> const& handler);
        void OpenURL(event_token const& token) noexcept;

        void ControlStrip_GetInfo(Windows::Foundation::IInspectable const&, CelestiaWinUI::InfoGetInfoArgs const&);
        void ControlStrip_ShowSubsystem(Windows::Foundation::IInspectable const&, CelestiaWinUI::InfoShowSubsystemArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        bool preserveMargin;
        Microsoft::UI::Xaml::Interop::IBindableObservableVector rootItem{ nullptr };
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs>> getInfoEvent;
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs>> showSubsystemEvent;
        event<Windows::Foundation::EventHandler<hstring>> openURLEvent;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct BrowserItemUserControl : BrowserItemUserControlT<BrowserItemUserControl, implementation::BrowserItemUserControl>
    {
    };
}
