//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "InstalledItemListPage.h"
#if __has_include("InstalledItemListParameter.g.cpp")
#include "InstalledItemListParameter.g.cpp"
#endif
#if __has_include("InstalledItemListPage.g.cpp")
#include "InstalledItemListPage.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::CelestiaUWP2::implementation
{
    InstalledItemListParameter::InstalledItemListParameter(CelestiaAppComponent::ResourceManager const& resourceManager, CelestiaUWP2::GetAddonHandler const& getAddonHandler) : resourceManager(resourceManager), getAddonHandler(getAddonHandler)
    {
    }

    ResourceManager InstalledItemListParameter::ResourceManager()
    {
        return resourceManager;
    }

    CelestiaUWP2::GetAddonHandler InstalledItemListParameter::GetAddonHandler()
    {
        return getAddonHandler;
    }

    InstalledItemListPage::InstalledItemListPage() : resourceManager(nullptr), getAddonHandler(nullptr)
    {
        items = single_threaded_observable_vector<CelestiaAppComponent::ResourceItem>();
    }

    void InstalledItemListPage::InitializeComponent()
    {
        InstalledItemListPageT::InitializeComponent();
        Title().Text(LocalizationHelper::Localize(L"Installed", L"Title for the list of installed add-ons"));
        EmptyHintText().Text(LocalizationHelper::Localize(L"Enhance Celestia with online add-ons", L""));
        EmptyHintButton().Content(box_value(LocalizationHelper::Localize(L"Get Add-ons", L"Open webpage for downloading add-ons")));
    }

    Collections::IObservableVector<CelestiaAppComponent::ResourceItem> InstalledItemListPage::Items()
    {
        return items;
    }

    event_token InstalledItemListPage::ItemSelected(EventHandler<CelestiaUWP2::InstalledItemListItemSelectedArgs> const& handler)
    {
        return itemSelectedEvent.add(handler);
    }

    void InstalledItemListPage::ItemSelected(event_token const& token)
    {
        itemSelectedEvent.remove(token);
    }

    void InstalledItemListPage::Reload()
    {
        LoadItems();
    }

    fire_and_forget InstalledItemListPage::LoadItems()
    {
        LoadingIndicator().Visibility(Visibility::Visible);
        EmptyHint().Visibility(Visibility::Collapsed);
        ItemList().Visibility(Visibility::Collapsed);
        auto installedItems = co_await resourceManager.InstalledItems();
        items.ReplaceAll(std::vector<CelestiaAppComponent::ResourceItem>{ installedItems.begin(), installedItems.end() });
        LoadingIndicator().Visibility(Visibility::Collapsed);
        if (items.Size() == 0)
            EmptyHint().Visibility(Visibility::Visible);
        else
            ItemList().Visibility(Visibility::Visible);
    }

    void InstalledItemListPage::ItemList_ItemClick(IInspectable const&, ItemClickEventArgs const& args)
    {
        auto item = args.ClickedItem().try_as<CelestiaAppComponent::ResourceItem>();
        if (item != nullptr)
            itemSelectedEvent(*this, make<InstalledItemListItemSelectedArgs>(item.as<CelestiaAppComponent::ResourceItem>()));
    }

    void InstalledItemListPage::GetAddonButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        getAddonHandler();
    }

    void InstalledItemListPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto parameter = e.Parameter().as<CelestiaUWP2::InstalledItemListParameter>();
        resourceManager = parameter.ResourceManager();
        getAddonHandler = parameter.GetAddonHandler();
        LoadItems();
    }
}
