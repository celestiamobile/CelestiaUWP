//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "ResourceManagerPage.h"
#if __has_include("ResourceManagerParameter.g.cpp")
#include "ResourceManagerParameter.g.cpp"
#endif
#if __has_include("ResourceManagerPage.g.cpp")
#include "ResourceManagerPage.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaComponent;
using namespace CelestiaAppComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

namespace winrt::CelestiaUWP2::implementation
{
    ResourceManagerParameter::ResourceManagerParameter(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, CelestiaAppComponent::ResourceManager const& resourceManager, CelestiaUWP2::GetAddonHandler const& getAddonHandler) : appCore(appCore), renderer(renderer), resourceManager(resourceManager), getAddonHandler(getAddonHandler)
    {
    }

    CelestiaAppCore ResourceManagerParameter::AppCore()
    {
        return appCore;
    }

    CelestiaRenderer ResourceManagerParameter::Renderer()
    {
        return renderer;
    }

    CelestiaAppComponent::ResourceManager ResourceManagerParameter::ResourceManager()
    {
        return resourceManager;
    }

    GetAddonHandler ResourceManagerParameter::GetAddonHandler()
    {
        return getAddonHandler;
    }

    ResourceManagerPage::ResourceManagerPage() : appCore(nullptr), renderer(nullptr), resourceManager(nullptr), getAddonHandler(nullptr)
    {
    }

    void ResourceManagerPage::Nav_BackRequested(IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const&)
    {
        if (!Container().CanGoBack()) return;

        /*
        if (currentItemControl != nullptr)
        {
            currentItemControl.CloseWebViewIfNeeded();
            currentItemControl = nullptr;
        }*/
        Container().GoBack();
        if (!Container().CanGoBack())
        {
            Nav().IsBackEnabled(false);
            if (auto installedList = Container().Content().try_as<CelestiaUWP2::InstalledItemListPage>(); installedList != nullptr)
                installedList.Reload();
        }
    }

    void ResourceManagerPage::ShowItem(CelestiaAppComponent::ResourceItem const& item)
    {
        /*
        if (currentItemControl != nullptr)
        {
            currentItemControl.CloseWebViewIfNeeded();
            currentItemControl = nullptr;
        }
        ResourceItemPage Page{ appCore, renderer, item, resourceManager, windowProvider };
        Container().Navigate(xaml_typename<CelestiaUWP2::CustomPage>(), Page);*/
        Nav().IsBackEnabled(true);
    }

    void ResourceManagerPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto parameter = e.Parameter().as<CelestiaUWP2::ResourceManagerParameter>();
        appCore = parameter.AppCore();
        renderer = parameter.Renderer();
        resourceManager = parameter.ResourceManager();
        getAddonHandler = parameter.GetAddonHandler();
        Container().Navigate(xaml_typename<CelestiaUWP2::InstalledItemListPage>(), CelestiaUWP2::InstalledItemListParameter(resourceManager, getAddonHandler));
        auto page = Container().Content().try_as<CelestiaUWP2::InstalledItemListPage>();
        if (page != nullptr)
        {
            page.ItemSelected([weak_this{ get_weak() }](IInspectable const&, CelestiaUWP2::InstalledItemListItemSelectedArgs const& args)
                {
                    auto strong_this{ weak_this.get() };
                    if (!strong_this) return;
                    strong_this->ShowItem(args.Item());
                });
        }
    }
}
