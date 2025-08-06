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

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        bool preserveMargin;
        Microsoft::UI::Xaml::Interop::IBindableObservableVector rootItem{ nullptr };
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs>> getInfoEvent;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct BrowserItemUserControl : BrowserItemUserControlT<BrowserItemUserControl, implementation::BrowserItemUserControl>
    {
    };
}
