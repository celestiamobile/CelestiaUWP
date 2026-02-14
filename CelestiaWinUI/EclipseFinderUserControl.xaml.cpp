// EclipseFinderUserControl.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"

#include "EclipseFinderUserControl.xaml.h"

#if __has_include("EclipseResult.g.cpp")
#include "EclipseResult.g.cpp"
#endif
#if __has_include("DateTimeStringConverter.g.cpp")
#include "DateTimeStringConverter.g.cpp"
#endif
#if __has_include("DateTimeReferenceDateTimeConverter.g.cpp")
#include "DateTimeReferenceDateTimeConverter.g.cpp"
#endif
#if __has_include("EclipseFinderUserControl.g.cpp")
#include "EclipseFinderUserControl.g.cpp"
#endif

#include <fmt/format.h>

using namespace winrt;
using namespace CelestiaComponent;
using namespace CelestiaAppComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    EclipseResult::EclipseResult(CelestiaEclipse const& eclipse, hstring const& displayName) : eclipse(eclipse), displayName(displayName) {}
    CelestiaEclipse EclipseResult::Eclipse() { return eclipse; }
    hstring EclipseResult::DisplayName() { return displayName; }

    EclipseFinderUserControl::EclipseFinderUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer)
    {
        availableObjects = single_threaded_observable_vector<SearchObjectEntry>({ SearchObjectEntry(CelestiaAppCore::LocalizedString(L"Earth", L"celestia-data"), L"Sol/Earth"), SearchObjectEntry(CelestiaAppCore::LocalizedString(L"Jupiter", L"celestia-data"), L"Sol/Jupiter")});
        eclipses = single_threaded_observable_vector<CelestiaWinUI::EclipseResult>();
        startTime = clock::now() - std::chrono::hours(24 * 365);
        endTime = clock::now();
    }

    void EclipseFinderUserControl::InitializeComponent()
    {
        EclipseFinderUserControlT::InitializeComponent();

        GoButton().Content(box_value(LocalizationHelper::Localize(L"Set Date and Go to Planet", L"Set date to eclipse date and view eclipse")));
        ComputeButton().Content(box_value(LocalizationHelper::Localize(L"Compute", L"Compute for eclipses")));
        StartTimeHint().Text(LocalizationHelper::Localize(L"Start time:", L"In eclipse finder, range of time to find eclipse in"));
        EndTimeHint().Text(LocalizationHelper::Localize(L"End time:", L"In eclipse finder, range of time to find eclipse in"));
        ObjectChooser().Header(box_value(LocalizationHelper::Localize(L"Object", L"In eclipse finder, object to find eclipse with, or in go to")));
        SolarEclipseButton().Content(box_value(LocalizationHelper::Localize(L"Solar", L"Solar eclipses.")));
        LunarEclipseButton().Content(box_value(LocalizationHelper::Localize(L"Lunar", L"Lunar eclipses.")));
        EmptyHintText().Text(LocalizationHelper::Localize(L"Find eclipses for an object in a time range", L""));
        auto object = availableObjects.GetAt(0);
        ObjectChooser().Text(object.Name());
        objectPath = object.Path();
    }

    DateTime EclipseFinderUserControl::StartTime()
    {
        return startTime;
    }

    void EclipseFinderUserControl::StartTime(DateTime const& value)
    {
        startTime = value;
    }

    DateTime EclipseFinderUserControl::EndTime()
    {
        return endTime;
    }

    void EclipseFinderUserControl::EndTime(DateTime const& value)
    {
        endTime = value;
    }

    bool EclipseFinderUserControl::FindLunar()
    {
        return findLunar;
    }

    void EclipseFinderUserControl::FindLunar(bool value)
    {
        findLunar = value;
    }

    bool EclipseFinderUserControl::FindSolar()
    {
        return findSolar;
    }

    void EclipseFinderUserControl::FindSolar(bool value)
    {
        findSolar = value;
    }

    Collections::IObservableVector<SearchObjectEntry> EclipseFinderUserControl::AvailableObjects()
    {
        return availableObjects;
    }

    Collections::IObservableVector<CelestiaWinUI::EclipseResult> EclipseFinderUserControl::Eclipses()
    {
        return eclipses;
    }

    fire_and_forget EclipseFinderUserControl::ComputeButton_Click(IInspectable const&, RoutedEventArgs const &)
    {
        if (finder != nullptr)
        {
            aborted = true;
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

        auto weak_this{ get_weak() };
        aborted = false;
        auto computedEclipses = co_await ComputeEclipsesAsync(body, kind, startTime, endTime);
        auto strong_this = weak_this.get();
        if (strong_this == nullptr || strong_this->aborted)
            co_return;

        strong_this->eclipses.ReplaceAll(std::vector<CelestiaWinUI::EclipseResult>(computedEclipses.begin(), computedEclipses.end()));
        strong_this->propertyChangedEvent(*strong_this, Data::PropertyChangedEventArgs(L"Eclipses"));

        strong_this->LoadingIndicator().Visibility(Visibility::Collapsed);
        if (strong_this->eclipses.Size() == 0)
        {
            strong_this->EmptyHintText().Text(LocalizationHelper::Localize(L"No eclipse is found for the given object in the time range", L""));
            strong_this->EmptyHintText().Visibility(Visibility::Visible);
        }
        else
        {
            strong_this->ResultList().Visibility(Visibility::Visible);
        }
        strong_this->ComputeButton().Content(box_value(LocalizationHelper::Localize(L"Compute", L"Compute for eclipses")));
    }

    IAsyncOperation<Collections::IVector<CelestiaWinUI::EclipseResult>> EclipseFinderUserControl::ComputeEclipsesAsync(CelestiaBody const body, CelestiaEclipseKind kind, DateTime const computeStartTime, DateTime const computeEndTime)
    {
        CelestiaEclipseFinder eclipseFinder{ body };
        finder = eclipseFinder;
        auto vectorResults = single_threaded_vector<CelestiaWinUI::EclipseResult>();
        winrt::apartment_context ui_thread;
        auto strong_this{ get_strong() };
        co_await winrt::resume_background();
        auto computedEclipses = eclipseFinder.Search(kind, computeStartTime, computeEndTime);
        for (const auto& eclipse : computedEclipses)
        {
            hstring displayName = to_hstring(fmt::format("{} - {}", to_string(eclipse.Occulter().Name()), to_string(eclipse.Receiver().Name())));
            vectorResults.Append(CelestiaWinUI::EclipseResult(eclipse, displayName));
        }
        co_await ui_thread;
        strong_this->finder = nullptr;
        co_return vectorResults;
    }

    void EclipseFinderUserControl::GoButton_Click(IInspectable const&, RoutedEventArgs const &)
    {
        auto selectedEclipse = ResultList().SelectedItem();
        if (selectedEclipse == nullptr)
            return;
        
        auto eclipse = selectedEclipse.as<CelestiaWinUI::EclipseResult>();
        renderer.EnqueueTask([weak_this{ get_weak() }, eclipse]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->appCore.Simulation().GoToEclipse(eclipse.Eclipse());
            });
    }

    event_token EclipseFinderUserControl::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChangedEvent.add(handler);
    }

    void EclipseFinderUserControl::PropertyChanged(event_token const& token) noexcept
    {
        propertyChangedEvent.remove(token);
    }

    void EclipseFinderUserControl::ObjectChooser_TextChanged(IInspectable const& sender, AutoSuggestBoxTextChangedEventArgs const& args)
    {
        if (args.Reason() != AutoSuggestionBoxTextChangeReason::UserInput)
            return;

        auto suggestBox = sender.as<AutoSuggestBox>();
        objectPath = suggestBox.Text();
    }

    void EclipseFinderUserControl::ObjectChooser_SuggestionChosen(IInspectable const& sender, AutoSuggestBoxSuggestionChosenEventArgs const& args)
    {
        auto suggestBox = sender.as<AutoSuggestBox>();
        auto selected = args.SelectedItem().as<SearchObjectEntry>();
        objectPath = selected.Path();
        suggestBox.Text(selected.Name());
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