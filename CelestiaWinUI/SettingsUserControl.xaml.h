// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "SettingsNavigationItemGroup.g.h"
#include "SettingsUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct SettingsNavigationItemGroup : SettingsNavigationItemGroupT<SettingsNavigationItemGroup>
    {
        SettingsNavigationItemGroup(hstring const& title, Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& items, bool showRestartHint);

        hstring Title();
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Items();
        bool ShowRestartHint();
    private:
        hstring title;
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> items;
        bool showRestartHint;
    };

    struct SettingsUserControl : SettingsUserControlT<SettingsUserControl>
    {
        SettingsUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaAppComponent::AppSettings const& appSettings, Windows::Storage::ApplicationDataContainer const& localSettings, Windows::Foundation::Collections::IVector<hstring> const& availableLanguages);
        void InitializeComponent();

        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::SettingsNavigationItemGroup> ItemGroups();

        void NavigationView_SelectionChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);
    private:
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::SettingsNavigationItemGroup> itemGroups;

        void ItemGroupSelected(CelestiaWinUI::SettingsNavigationItemGroup const& itemGroup);
   };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct SettingsNavigationItemGroup : SettingsNavigationItemGroupT<SettingsNavigationItemGroup, implementation::SettingsNavigationItemGroup>
    {
    };

    struct SettingsUserControl : SettingsUserControlT<SettingsUserControl, implementation::SettingsUserControl>
    {
    };
}
