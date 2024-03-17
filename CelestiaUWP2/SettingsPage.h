//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "SettingsParameter.g.h"
#include "SettingsNavigationItemGroup.g.h"
#include "SettingsPage.g.h"

namespace winrt::CelestiaUWP2::implementation
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

    struct SettingsParameter : SettingsParameterT<SettingsParameter>
    {
        SettingsParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaAppComponent::AppSettings const& appSettings, Windows::Storage::ApplicationDataContainer const& localSettings, Windows::Foundation::Collections::IVector<hstring> const& availableLanguages);
        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
        CelestiaAppComponent::AppSettings AppSettings();
        Windows::Storage::ApplicationDataContainer LocalSettings();
        Windows::Foundation::Collections::IVector<hstring> AvailableLanguages();
    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaAppComponent::AppSettings appSettings;
        Windows::Storage::ApplicationDataContainer localSettings;
        Windows::Foundation::Collections::IVector<hstring> availableLanguages;
    };

    struct SettingsPage : SettingsPageT<SettingsPage>
    {
        SettingsPage();

        void LoadPage(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaAppComponent::AppSettings const& appSettings, Windows::Storage::ApplicationDataContainer const& localSettings, Windows::Foundation::Collections::IVector<hstring> const& availableLanguages);

        Windows::Foundation::Collections::IObservableVector<CelestiaUWP2::SettingsNavigationItemGroup> ItemGroups();

        void NavigationView_SelectionChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        Windows::Foundation::Collections::IObservableVector<CelestiaUWP2::SettingsNavigationItemGroup> itemGroups;

        void ItemGroupSelected(CelestiaUWP2::SettingsNavigationItemGroup const& itemGroup);
   };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct SettingsNavigationItemGroup : SettingsNavigationItemGroupT<SettingsNavigationItemGroup, implementation::SettingsNavigationItemGroup>
    {
    };

    struct SettingsParameter : SettingsParameterT<SettingsParameter, implementation::SettingsParameter>
    {
    };

    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}
