//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "InstalledItemListItemSelectedArgs.g.h"
#include "InstalledItemListParameter.g.h"
#include "InstalledItemListPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct InstalledItemListParameter : InstalledItemListParameterT<InstalledItemListParameter>
    {
        InstalledItemListParameter(CelestiaAppComponent::ResourceManager const& resourceManager, CelestiaUWP2::GetAddonHandler const& getAddonHandler);
        CelestiaAppComponent::ResourceManager ResourceManager();
        CelestiaUWP2::GetAddonHandler GetAddonHandler();
    private:
        CelestiaAppComponent::ResourceManager resourceManager;
        CelestiaUWP2::GetAddonHandler getAddonHandler;
    };

    struct InstalledItemListItemSelectedArgs : InstalledItemListItemSelectedArgsT<InstalledItemListItemSelectedArgs>
    {
        InstalledItemListItemSelectedArgs(CelestiaAppComponent::ResourceItem const& item) : item(item) {}
        CelestiaAppComponent::ResourceItem Item() { return item; }
    private:
        CelestiaAppComponent::ResourceItem item;
    };

    struct InstalledItemListPage : InstalledItemListPageT<InstalledItemListPage>
    {
        InstalledItemListPage();
        void InitializeComponent();

        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::ResourceItem> Items();

        event_token ItemSelected(Windows::Foundation::EventHandler<CelestiaUWP2::InstalledItemListItemSelectedArgs> const&);
        void ItemSelected(event_token const&);

        void Reload();

        void ItemList_ItemClick(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::ItemClickEventArgs const&);
        void GetAddonButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        CelestiaAppComponent::ResourceManager resourceManager;
        CelestiaUWP2::GetAddonHandler getAddonHandler;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::ResourceItem> items;
        event<Windows::Foundation::EventHandler<CelestiaUWP2::InstalledItemListItemSelectedArgs>> itemSelectedEvent;

        fire_and_forget LoadItems();
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct InstalledItemListParameter : InstalledItemListParameterT<InstalledItemListParameter, implementation::InstalledItemListParameter>
    {
    };

    struct InstalledItemListPage : InstalledItemListPageT<InstalledItemListPage, implementation::InstalledItemListPage>
    {
    };
}
