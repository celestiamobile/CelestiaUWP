//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "InfoParameter.g.h"
#include "InfoPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct InfoParameter : InfoParameterT<InfoParameter>
    {
        InfoParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaComponent::CelestiaSelection const& selection);
        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
        CelestiaComponent::CelestiaSelection Selection();
    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaComponent::CelestiaSelection selection;
    };

    struct InfoPage : InfoPageT<InfoPage>
    {
        InfoPage();

        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserInputAction> Actions();
        void ActionButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaComponent::CelestiaSelection selection;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserInputAction> actions;
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct InfoParameter : InfoParameterT<InfoParameter, implementation::InfoParameter>
    {
    };

    struct InfoPage : InfoPageT<InfoPage, implementation::InfoPage>
    {
    };
}
