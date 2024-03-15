//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "EclipseFinderPage.h"

#if __has_include("EclipseResult.g.cpp")
#include "EclipseResult.g.cpp"
#endif
#if __has_include("EclipseFinderParameter.g.cpp")
#include "EclipseFinderParameter.g.cpp"
#endif
#if __has_include("DateTimeStringConverter.g.cpp")
#include "DateTimeStringConverter.g.cpp"
#endif
#if __has_include("DateTimeReferenceDateTimeConverter.g.cpp")
#include "DateTimeReferenceDateTimeConverter.g.cpp"
#endif
#if __has_include("EclipseFinderPage.g.cpp")
#include "EclipseFinderPage.g.cpp"
#endif

#include <fmt/format.h>
#include <fmt/xchar.h>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::CelestiaUWP2::implementation
{
    EclipseResult::EclipseResult(CelestiaEclipse const& eclipse, hstring const& displayName) : eclipse(eclipse), displayName(displayName) {}
    CelestiaEclipse EclipseResult::Eclipse() { return eclipse; }
    hstring EclipseResult::DisplayName() { return displayName; }

    EclipseFinderParameter::EclipseFinderParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer) {}
    CelestiaComponent::CelestiaAppCore EclipseFinderParameter::AppCore() { return appCore; }
    CelestiaComponent::CelestiaRenderer EclipseFinderParameter::Renderer() { return renderer;  }

    EclipseFinderPage::EclipseFinderPage() : appCore(nullptr), renderer(nullptr)
    {
        availableObjects = single_threaded_observable_vector<SearchObjectEntry>({ SearchObjectEntry(CelestiaAppCore::LocalizedString(L"Earth", L"celestia-data"), L"Sol/Earth"), SearchObjectEntry(CelestiaAppCore::LocalizedString(L"Jupiter", L"celestia-data"), L"Sol/Jupiter")});
        eclipses = single_threaded_observable_vector<CelestiaUWP2::EclipseResult>();
        startTime = clock::now();
        endTime = clock::now();
    }

    void EclipseFinderPage::InitializeComponent()
    {
        EclipseFinderPageT::InitializeComponent();

        GoButton().Content(box_value(LocalizationHelper::Localize(L"Set Date and Go to Planet", L"Set date to eclipse date and view eclipse")));
        ComputeButton().Content(box_value(LocalizationHelper::Localize(L"Compute", L"Compute for eclipses")));
        StartTimeHint().Text(LocalizationHelper::Localize(L"Start time:", L"In eclipse finder, range of time to find eclipse in"));
        EndTimeHint().Text(LocalizationHelper::Localize(L"End time:", L"In eclipse finder, range of time to find eclipse in"));
        ObjectHint().Text(LocalizationHelper::Localize(L"Object:", L"In Go to, Windows, In eclipse finder, object to find eclipse with"));
        SolarEclipseButton().Content(box_value(LocalizationHelper::Localize(L"Solar", L"Solar eclipses.")));
        LunarEclipseButton().Content(box_value(LocalizationHelper::Localize(L"Lunar", L"Lunar eclipses.")));
        EmptyHintText().Text(LocalizationHelper::Localize(L"Find eclipses for an object in a time range", L""));
        auto object = availableObjects.GetAt(0);
        ObjectChooser().Text(object.Name());
        objectPath = object.Path();
    }

    DateTime EclipseFinderPage::StartTime()
    {
        return startTime;
    }

    void EclipseFinderPage::StartTime(DateTime const& value)
    {
        startTime = value;
    }

    DateTime EclipseFinderPage::EndTime()
    {
        return endTime;
    }

    void EclipseFinderPage::EndTime(DateTime const& value)
    {
        endTime = value;
    }

    bool EclipseFinderPage::FindLunar()
    {
        return findLunar;
    }

    void EclipseFinderPage::FindLunar(bool value)
    {
        findLunar = value;
    }

    bool EclipseFinderPage::FindSolar()
    {
        return findSolar;
    }

    void EclipseFinderPage::FindSolar(bool value)
    {
        findSolar = value;
    }

    Collections::IObservableVector<SearchObjectEntry> EclipseFinderPage::AvailableObjects()
    {
        return availableObjects;
    }

    Collections::IObservableVector<CelestiaUWP2::EclipseResult> EclipseFinderPage::Eclipses()
    {
        return eclipses;
    }

    fire_and_forget EclipseFinderPage::ComputeButton_Click(IInspectable const&, RoutedEventArgs const &)
    {
        if (finder != nullptr)
        {
            finder.Abort();
            finder = nullptr;
            return;
        }
        if (endTime <= startTime || (!findLunar && !findSolar))
            return;
        
        CelestiaEclipseKind kind = static_cast<CelestiaEclipseKind>(0);
        if (findSolar)
            kind = static_cast<CelestiaEclipseKind>(static_cast<uint32_t>(kind) | static_cast<uint32_t>(CelestiaEclipseKind::Solar));
        if (findLunar)
            kind = static_cast<CelestiaEclipseKind>(static_cast<uint32_t>(kind) | static_cast<uint32_t>(CelestiaEclipseKind::Lunar));
 
        auto selection = appCore.Simulation().Find(objectPath);
        if (selection == nullptr || selection.IsEmpty())
            return;

        auto body = selection.Object().try_as<CelestiaBody>();
        if (body == nullptr)
            return;

        ComputeButton().Content(box_value(LocalizationHelper::Localize(L"Cancel", L"")));
        LoadingIndicator().Visibility(Visibility::Visible);
        EmptyHintText().Visibility(Visibility::Collapsed);
        ResultList().Visibility(Visibility::Collapsed);
        auto computedEclipses = co_await ComputeEclipsesAsync(body, kind, startTime, endTime);
        eclipses.ReplaceAll(std::vector<CelestiaUWP2::EclipseResult>(computedEclipses.begin(), computedEclipses.end()));
        propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"Eclipses"));

        LoadingIndicator().Visibility(Visibility::Collapsed);
        if (eclipses.Size() == 0)
        {
            EmptyHintText().Text(LocalizationHelper::Localize(L"No eclipse is found for the given object in the time range", L""));
            EmptyHintText().Visibility(Visibility::Visible);
        }
        else
        {
            ResultList().Visibility(Visibility::Visible);
        }
        ComputeButton().Content(box_value(LocalizationHelper::Localize(L"Compute", L"Compute for eclipses")));
    }

    IAsyncOperation<Collections::IVector<CelestiaUWP2::EclipseResult>> EclipseFinderPage::ComputeEclipsesAsync(CelestiaBody const body, CelestiaEclipseKind kind, DateTime const computeStartTime, DateTime const computeEndTime)
    {
        CelestiaEclipseFinder eclipseFinder{ body };
        finder = eclipseFinder;
        auto vectorResults = single_threaded_vector<CelestiaUWP2::EclipseResult>();
        winrt::apartment_context ui_thread;
        co_await winrt::resume_background();
        auto computedEclipses = eclipseFinder.Search(kind, computeStartTime, computeEndTime);
        for (const auto& eclipse : computedEclipses)
        {
            hstring displayName = hstring(fmt::format(L"{} - {}", std::wstring(eclipse.Occulter().Name()), std::wstring(eclipse.Receiver().Name())));
            vectorResults.Append(CelestiaUWP2::EclipseResult(eclipse, displayName));
        }
        co_await ui_thread;
        finder = nullptr;
        co_return vectorResults;
    }

    void EclipseFinderPage::GoButton_Click(IInspectable const&, RoutedEventArgs const &)
    {
        auto selectedEclipse = ResultList().SelectedItem();
        if (selectedEclipse == nullptr)
            return;
        
        auto eclipse = selectedEclipse.as<CelestiaEclipse>();
        renderer.EnqueueTask([this, eclipse]()
            {
                appCore.Simulation().GoToEclipse(eclipse);
            });
    }

    event_token EclipseFinderPage::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChangedEvent.add(handler);
    }

    void EclipseFinderPage::PropertyChanged(event_token const& token) noexcept
    {
        propertyChangedEvent.remove(token);
    }

    void EclipseFinderPage::ObjectChooser_TextChanged(IInspectable const& sender, AutoSuggestBoxTextChangedEventArgs const& args)
    {
        if (args.Reason() != AutoSuggestionBoxTextChangeReason::UserInput)
            return;

        auto suggestBox = sender.as<AutoSuggestBox>();
        objectPath = suggestBox.Text();
    }

    void EclipseFinderPage::ObjectChooser_SuggestionChosen(IInspectable const& sender, AutoSuggestBoxSuggestionChosenEventArgs const& args)
    {
        auto suggestBox = sender.as<AutoSuggestBox>();
        auto selected = args.SelectedItem().as<SearchObjectEntry>();
        objectPath = selected.Path();
        suggestBox.Text(selected.Name());
    }

    void EclipseFinderPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto parameter = e.Parameter().as<CelestiaUWP2::EclipseFinderParameter>();
        appCore = parameter.AppCore();
        renderer = parameter.Renderer();
    }

    IInspectable DateTimeStringConverter::Convert(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const&, IInspectable const&, hstring const&)
    {
        auto unboxed = unbox_value_or<DateTime>(value, clock::now());
        return box_value(dateFormatter.Format(unboxed));
    }

    IInspectable DateTimeStringConverter::ConvertBack(IInspectable const&, Windows::UI::Xaml::Interop::TypeName const&, IInspectable const&, hstring const&)
    {
        // Not used for now
        return box_value(clock::now());
    }

    IInspectable DateTimeReferenceDateTimeConverter::Convert(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const&, IInspectable const&, hstring const&)
    {
        auto unboxed = unbox_value_or<DateTime>(value, clock::now());
        return IReference<DateTime>{ unboxed };
    }

    IInspectable DateTimeReferenceDateTimeConverter::ConvertBack(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const&, IInspectable const&, hstring const&)
    {
        auto unboxed = unbox_value_or<IReference<DateTime>>(value, IReference<DateTime>(clock::now()));
        if (!unboxed)
            return box_value(clock::now());
        return box_value(unboxed.Value());
    }
}