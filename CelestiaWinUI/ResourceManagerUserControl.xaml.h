// ResourceManagerUserControl.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "ResourceManagerUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct ResourceManagerUserControl : ResourceManagerUserControlT<ResourceManagerUserControl>
    {
        ResourceManagerUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaAppComponent::ResourceManager const& resourceManager, CelestiaWinUI::WebWindowProvider const& windowProvider, CelestiaWinUI::GetAddonHandler const& getAddonHandler);
        void InitializeComponent();

        void Nav_BackRequested(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaAppComponent::ResourceManager resourceManager;
        CelestiaWinUI::WebWindowProvider windowProvider;
        CelestiaWinUI::ResourceItemUserControl currentItemControl{ nullptr };
        CelestiaWinUI::InstalledItemListUserControl installedList;

        void ShowItem(CelestiaAppComponent::ResourceItem const& item);
   };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct ResourceManagerUserControl : ResourceManagerUserControlT<ResourceManagerUserControl, implementation::ResourceManagerUserControl>
    {
    };
}
