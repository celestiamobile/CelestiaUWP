// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "TourGuideWindow.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct TourGuideWindow : TourGuideWindowT<TourGuideWindow>
    {
        TourGuideWindow(array_view<CelestiaComponent::CelestiaDestination const> destinations, CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        Windows::Foundation::Collections::IObservableVector<CelestiaComponent::CelestiaDestination> Destinations();
        void GoButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        int32_t SelectedIndex();
        void SelectedIndex(int32_t);
        void DestinationSelection_SelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Windows::Foundation::Collections::IObservableVector<CelestiaComponent::CelestiaDestination> destinations;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct TourGuideWindow : TourGuideWindowT<TourGuideWindow, implementation::TourGuideWindow>
    {
    };
}
