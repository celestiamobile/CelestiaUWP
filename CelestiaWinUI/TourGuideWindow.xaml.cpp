// TourGuideWindow.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "TourGuideWindow.xaml.h"
#if __has_include("TourGuideWindow.g.cpp")
#include "TourGuideWindow.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    TourGuideWindow::TourGuideWindow(array_view<CelestiaDestination const> dests, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer)
    {
        destinations = single_threaded_observable_vector<CelestiaDestination>();
        for (const auto& destination : dests)
            destinations.Append(destination);
    }

    void TourGuideWindow::InitializeComponent()
    {
        TourGuideWindowT::InitializeComponent();

        TitleText().Text(LocalizationHelper::Localize(L"Select your destination:", L"Select a destination in tour guide"));
        GoButton().Content(box_value(LocalizationHelper::Localize(L"Go", L"Go to an object")));
    }

    Collections::IObservableVector<CelestiaDestination> TourGuideWindow::Destinations()
    {
        return destinations;
    }

    void TourGuideWindow::DestinationSelection_SelectionChanged(IInspectable const&, Controls::SelectionChangedEventArgs const&)
    {
        auto item = DestinationSelection().SelectedItem();
        if (item == nullptr)
        {
            DescriptionText().Text(L"");
            return;
        }
        auto destination = item.as<CelestiaDestination>();
        DescriptionText().Text(destination.Content());
    }

    void TourGuideWindow::GoButton_Click(IInspectable const&, RoutedEventArgs const &)
    {
        auto item = DestinationSelection().SelectedItem();
        if (item == nullptr) return;
        auto destination = item.as<CelestiaDestination>();
        renderer.EnqueueTask([this, destination]()
            {
                appCore.Simulation().GoToDestination(destination);
            });
    }
}