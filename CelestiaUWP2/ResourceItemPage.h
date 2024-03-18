//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "ResourceItemParameter.g.h"
#include "ResourceItemPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct ResourceItemParameter : ResourceItemParameterT<ResourceItemParameter>
    {
        ResourceItemParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaAppComponent::ResourceItem const& item, CelestiaAppComponent::ResourceManager const& resourceManager);
        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
        CelestiaAppComponent::ResourceItem Item();
        CelestiaAppComponent::ResourceManager ResourceManager();
    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaAppComponent::ResourceItem item;
        CelestiaAppComponent::ResourceManager resourceManager;
    };

    struct ResourceItemPage : ResourceItemPageT<ResourceItemPage>
    {
        ResourceItemPage();

        void Load();

        fire_and_forget ActionButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void GoButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

        void ResourceManager_DownloadProgressUpdate(Windows::Foundation::IInspectable const&, CelestiaAppComponent::ResourceManagerDownloadProgressArgs const&);
        void ResourceManager_DownloadSuccess(Windows::Foundation::IInspectable const&, CelestiaAppComponent::ResourceManagerDownloadSuccessArgs const&);
        fire_and_forget ResourceManager_DownloadFailure(Windows::Foundation::IInspectable const&, CelestiaAppComponent::ResourceManagerDownloadFailureArgs const&);

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaAppComponent::ResourceItem item;
        CelestiaAppComponent::ResourceManager resourceManager;

        void UpdateState();
        fire_and_forget ReloadItem();
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct ResourceItemParameter : ResourceItemParameterT<ResourceItemParameter, implementation::ResourceItemParameter>
    {
    };

    struct ResourceItemPage : ResourceItemPageT<ResourceItemPage, implementation::ResourceItemPage>
    {
    };
}
