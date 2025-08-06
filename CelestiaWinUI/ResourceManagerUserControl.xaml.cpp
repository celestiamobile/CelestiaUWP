// ResourceManagerUserControl.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "ResourceManagerUserControl.xaml.h"
#if __has_include("ResourceManagerUserControl.g.cpp")
#include "ResourceManagerUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaComponent;
using namespace CelestiaAppComponent;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;

namespace winrt::CelestiaWinUI::implementation
{
    ResourceManagerUserControl::ResourceManagerUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, ResourceManager const& resourceManager, CelestiaWinUI::WebWindowProvider const& windowProvider, CelestiaWinUI::GetAddonHandler const& getAddonHandler) : appCore(appCore), renderer(renderer), resourceManager(resourceManager), windowProvider(windowProvider), installedList(resourceManager, getAddonHandler)
    {
    }

    void ResourceManagerUserControl::InitializeComponent()
    {
        ResourceManagerUserControlT::InitializeComponent();

        installedList.ItemSelected([weak_this{ get_weak() }](IInspectable const&, InstalledItemListItemSelectedArgs const& args)
            {
                auto strong_this { weak_this.get() };
                if (!strong_this) return;
                strong_this->ShowItem(args.Item());
            });

        Container().Navigate(xaml_typename<CelestiaWinUI::CustomPage>(), installedList);
    }

    void ResourceManagerUserControl::Nav_BackRequested(IInspectable const&, Controls::NavigationViewBackRequestedEventArgs const&)
    {
        if (!Container().CanGoBack()) return;

        if (currentItemControl != nullptr)
        {
            currentItemControl.CloseWebViewIfNeeded();
            currentItemControl = nullptr;
        }
        Container().GoBack();
        if (!Container().CanGoBack())
        {
            Nav().IsBackEnabled(false);
            installedList.Reload();
        }
    }

    void ResourceManagerUserControl::ShowItem(CelestiaAppComponent::ResourceItem const& item)
    {
        if (currentItemControl != nullptr)
        {
            currentItemControl.CloseWebViewIfNeeded();
            currentItemControl = nullptr;
        }
        ResourceItemUserControl userControl{ appCore, renderer, item, resourceManager, windowProvider };
        Container().Navigate(xaml_typename<CelestiaWinUI::CustomPage>(), userControl);
        Nav().IsBackEnabled(true);
        currentItemControl = userControl;
    }
}
