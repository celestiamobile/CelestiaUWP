//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "ResourceManagerParameter.g.h"
#include "ResourceManagerPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct ResourceManagerParameter : ResourceManagerParameterT<ResourceManagerParameter>
    {
        ResourceManagerParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaAppComponent::ResourceManager const& resourceManager, CelestiaUWP2::GetAddonHandler const& getAddonHandler);
        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
        CelestiaAppComponent::ResourceManager ResourceManager();
        CelestiaUWP2::GetAddonHandler GetAddonHandler();
    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaAppComponent::ResourceManager resourceManager;
        CelestiaUWP2::GetAddonHandler getAddonHandler;
    };

    struct ResourceManagerPage : ResourceManagerPageT<ResourceManagerPage>
    {
        ResourceManagerPage();

        void Nav_BackRequested(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const&);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaAppComponent::ResourceManager resourceManager;
        CelestiaUWP2::GetAddonHandler getAddonHandler;

        void ShowItem(CelestiaAppComponent::ResourceItem const& item);
   };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct ResourceManagerParameter : ResourceManagerParameterT<ResourceManagerParameter, implementation::ResourceManagerParameter>
    {
    };

    struct ResourceManagerPage : ResourceManagerPageT<ResourceManagerPage, implementation::ResourceManagerPage>
    {
    };
}
