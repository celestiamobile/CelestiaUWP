//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "TourGuideParameter.g.h"
#include "TourGuidePage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct TourGuideParameter : TourGuideParameterT<TourGuideParameter>
    {
        TourGuideParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, Windows::Foundation::Collections::IVector<CelestiaComponent::CelestiaDestination> const& destinations);
        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
        Windows::Foundation::Collections::IVector<CelestiaComponent::CelestiaDestination> Destinations();
    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Windows::Foundation::Collections::IVector<CelestiaComponent::CelestiaDestination> destinations;
    };

    struct TourGuidePage : TourGuidePageT<TourGuidePage>
    {
        TourGuidePage();
        void InitializeComponent();

        Windows::UI::Xaml::Interop::IBindableObservableVector Destinations();
        void GoButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void DestinationSelection_SelectionChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Windows::UI::Xaml::Interop::IBindableObservableVector destinations;
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct TourGuideParameter : TourGuideParameterT<TourGuideParameter, implementation::TourGuideParameter>
    {
    };

    struct TourGuidePage : TourGuidePageT<TourGuidePage, implementation::TourGuidePage>
    {
    };
}
