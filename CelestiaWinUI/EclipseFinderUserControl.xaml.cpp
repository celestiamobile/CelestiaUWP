// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "EclipseFinderUserControl.xaml.h"
#if __has_include("EclipseFinderUserControl.g.cpp")
#include "EclipseFinderUserControl.g.cpp"
#endif
#if __has_include("DateTimeStringConverter.g.cpp")
#include "DateTimeStringConverter.g.cpp"
#endif
#if __has_include("DateTimeReferenceDateTimeConverter.g.cpp")
#include "DateTimeReferenceDateTimeConverter.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    EclipseFinderUserControl::EclipseFinderUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer)
    {
        availableObjects = single_threaded_observable_vector<hstring>({ CelestiaAppCore::LocalizedString(L"Earth", L"celestia-data"), CelestiaAppCore::LocalizedString(L"Jupiter", L"celestia-data") });
        eclipses = single_threaded_observable_vector<CelestiaEclipse>();
        startTime = clock::now();
        endTime = clock::now();

        InitializeComponent();

        GoButton().Content(box_value(LocalizationHelper::Localize(L"Set Date and Go to Planet")));
        ComputeButton().Content(box_value(LocalizationHelper::Localize(L"Compute")));
        StartTimeHint().Text(LocalizationHelper::Localize(L"Start time:"));
        EndTimeHint().Text(LocalizationHelper::Localize(L"End time:"));
        ObjectHint().Text(LocalizationHelper::Localize(L"Object:"));
        SolarEclipseButton().Content(box_value(LocalizationHelper::Localize(L"Solar")));
        LunarEclipseButton().Content(box_value(LocalizationHelper::Localize(L"Lunar")));    
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

    Collections::IObservableVector<hstring> EclipseFinderUserControl::AvailableObjects()
    {
        return availableObjects;
    }

    Collections::IObservableVector<CelestiaEclipse> EclipseFinderUserControl::Eclipses()
    {
        return eclipses;
    }

    hstring EclipseFinderUserControl::GetEclipseDescription(CelestiaBody const& occulter, CelestiaBody const& received)
    {
        return occulter.Name() + L" - " + received.Name();
    }

    fire_and_forget EclipseFinderUserControl::ComputeButton_Click(IInspectable const&, RoutedEventArgs const &)
    {
        if (finder != nullptr)
        {
            finder.Abort();
            finder = nullptr;
            ComputeButton().Content(box_value(LocalizationHelper::Localize(L"Compute")));
            return;
        }
        if (endTime <= startTime || (!findLunar && !findSolar))
            return;
        
        CelestiaEclipseKind kind = CelestiaEclipseKind::SolarAndLunar;
        if (findLunar && !findSolar)
            kind = CelestiaEclipseKind::Lunar;
        else if (!findLunar && findSolar)
            kind = CelestiaEclipseKind::Solar;
        
        auto selectedObject = ObjectChooser().SelectedItem();
        if (selectedObject == nullptr)
            return;
            
        auto selection = appCore.Simulation().Find(selectedObject.as<hstring>());
        if (selection == nullptr || selection.IsEmpty())
            return;

        auto body = selection.Object().try_as<CelestiaBody>();
        if (body == nullptr)
            return;

        auto computedEclipses = co_await ComputeEclipsesAsync(body, kind, startTime, endTime);
        eclipses.ReplaceAll(std::vector<CelestiaEclipse>(computedEclipses.begin(), computedEclipses.end()));
        propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"Eclipses"));

        ComputeButton().Content(box_value(LocalizationHelper::Localize(L"Compute")));
    }

    IAsyncOperation<Collections::IVector<CelestiaEclipse>> EclipseFinderUserControl::ComputeEclipsesAsync(CelestiaBody const body, CelestiaEclipseKind kind, DateTime const computeStartTime, DateTime const computeEndTime)
    {
        CelestiaEclipseFinder eclipseFinder{ body };
        finder = eclipseFinder;
        auto vectorResults = single_threaded_vector<CelestiaEclipse>();
        ComputeButton().Content(box_value(LocalizationHelper::Localize(L"Cancel")));
        winrt::apartment_context ui_thread;
        co_await winrt::resume_background();
        auto computedEclipses = eclipseFinder.Search(kind, computeStartTime, computeEndTime);
        co_await ui_thread;
        vectorResults.ReplaceAll(computedEclipses);
        finder = nullptr;
        co_return vectorResults;
    }

    void EclipseFinderUserControl::GoButton_Click(IInspectable const&, RoutedEventArgs const &)
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

    event_token EclipseFinderUserControl::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChangedEvent.add(handler);
    }

    void EclipseFinderUserControl::PropertyChanged(event_token const& token) noexcept
    {
        propertyChangedEvent.remove(token);
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