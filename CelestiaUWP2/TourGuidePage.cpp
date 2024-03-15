//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "TourGuidePage.h"
#if __has_include("TourGuideParameter.g.cpp")
#include "TourGuideParameter.g.cpp"
#endif
#if __has_include("TourGuidePage.g.cpp")
#include "TourGuidePage.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

namespace winrt::CelestiaUWP2::implementation
{
    TourGuideParameter::TourGuideParameter(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, Collections::IVector<CelestiaDestination> const& destinations)
        : appCore(appCore), renderer(renderer), destinations(destinations)
    {
    }

    Collections::IVector<CelestiaDestination>  TourGuideParameter::Destinations()
    {
        return destinations;
    }

    CelestiaAppCore TourGuideParameter::AppCore()
    {
        return appCore;
    }

    CelestiaRenderer TourGuideParameter::Renderer()
    {
        return renderer;
    }

    TourGuidePage::TourGuidePage() : appCore(nullptr), renderer(nullptr), destinations(nullptr)
    {
    }

    void TourGuidePage::InitializeComponent()
    {
        TourGuidePageT::InitializeComponent();

        TitleText().Text(LocalizationHelper::Localize(L"Select your destination:", L"Select a destination in tour guide"));
        GoButton().Content(box_value(LocalizationHelper::Localize(L"Go", L"Go to an object")));
    }

    Interop::IBindableObservableVector TourGuidePage::Destinations()
    {
        return destinations;
    }

    void TourGuidePage::DestinationSelection_SelectionChanged(IInspectable const&, Controls::SelectionChangedEventArgs const&)
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

    void TourGuidePage::GoButton_Click(IInspectable const&, RoutedEventArgs const &)
    {
        auto item = DestinationSelection().SelectedItem();
        if (item == nullptr) return;
        auto destination = item.as<CelestiaDestination>();
        renderer.EnqueueTask([this, destination]()
            {
                appCore.Simulation().GoToDestination(destination);
            });
    }

    void TourGuidePage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto parameter = e.Parameter().as<CelestiaUWP2::TourGuideParameter>();
        appCore = parameter.AppCore();
        renderer = parameter.Renderer();
        auto destinationList = parameter.Destinations();
        auto list = single_threaded_observable_vector<CelestiaDestination>({ destinationList.begin(), destinationList.end() });
        destinations = DataHelper::ConvertToBindableDestinations(list);
    }
}