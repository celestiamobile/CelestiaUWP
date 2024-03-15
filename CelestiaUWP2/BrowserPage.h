//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "BrowserParameter.g.h"
#include "BrowserPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct BrowserParameter : BrowserParameterT<BrowserParameter>
    {
        BrowserParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
    };

    struct BrowserPage : BrowserPageT<BrowserPage>
    {
        BrowserPage();

        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItemTab> RootItems();
        Windows::UI::Xaml::Interop::IBindableObservableVector RootItem();
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserAction> Actions();

        void NavigationView_SelectionChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);
        void Nav_BackRequested(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const&);
        void ActionButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItemTab> rootItems;
        Windows::UI::Xaml::Interop::IBindableObservableVector rootItem{ nullptr };
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserAction> actions;

        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> propertyChangedEvent;

        void LoadData();
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct BrowserParameter : BrowserParameterT<BrowserParameter, implementation::BrowserParameter>
    {
    };

    struct BrowserPage : BrowserPageT<BrowserPage, implementation::BrowserPage>
    {
    };
}
