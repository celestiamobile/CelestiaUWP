// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "ResourceItemUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct ResourceItemUserControl : ResourceItemUserControlT<ResourceItemUserControl>
    {
        ResourceItemUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaAppComponent::ResourceItem const& item, CelestiaAppComponent::ResourceManager const& resourceManager, CelestiaWinUI::WebWindowProvider const& windowProvider);
        void InitializeComponent();

        fire_and_forget ActionButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void GoButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

        void ResourceManager_DownloadProgressUpdate(Windows::Foundation::IInspectable const&, CelestiaAppComponent::ResourceManagerDownloadProgressArgs const&);
        void ResourceManager_DownloadSuccess(Windows::Foundation::IInspectable const&, CelestiaAppComponent::ResourceManagerDownloadSuccessArgs const&);
        fire_and_forget ResourceManager_DownloadFailure(Windows::Foundation::IInspectable const&, CelestiaAppComponent::ResourceManagerDownloadFailureArgs const&);

        void CloseWebViewIfNeeded();

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaAppComponent::ResourceItem item;
        CelestiaAppComponent::ResourceManager resourceManager;
        CelestiaWinUI::WebWindowProvider windowProvider;
        CelestiaWinUI::SafeWebUserControl webPage{ nullptr };

        void UpdateState();
        fire_and_forget ReloadItem();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct ResourceItemUserControl : ResourceItemUserControlT<ResourceItemUserControl, implementation::ResourceItemUserControl>
    {
    };
}
