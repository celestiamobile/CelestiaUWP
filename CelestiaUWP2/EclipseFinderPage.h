//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "EclipseFinderParameter.g.h"
#include "EclipseResult.g.h"
#include "DateTimeStringConverter.g.h"
#include "DateTimeReferenceDateTimeConverter.g.h"
#include "EclipseFinderPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct EclipseResult : EclipseResultT<EclipseResult>
    {
        EclipseResult(CelestiaComponent::CelestiaEclipse const& eclipse, hstring const& displayName);
        CelestiaComponent::CelestiaEclipse Eclipse();
        hstring DisplayName();
    private:
        CelestiaComponent::CelestiaEclipse eclipse;
        hstring displayName;
    };

    struct EclipseFinderParameter : EclipseFinderParameterT<EclipseFinderParameter>
    {
        EclipseFinderParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
    };

    struct DateTimeStringConverter : DateTimeStringConverterT<DateTimeStringConverter>
    {
        DateTimeStringConverter() = default;

        Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);

    private:
        Windows::Globalization::DateTimeFormatting::DateTimeFormatter dateFormatter{ L"shortdate shorttime" };
    };

    struct DateTimeReferenceDateTimeConverter : DateTimeReferenceDateTimeConverterT<DateTimeReferenceDateTimeConverter>
    {
        DateTimeReferenceDateTimeConverter() = default;


        Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };

    struct EclipseFinderPage : EclipseFinderPageT<EclipseFinderPage>
    {
        EclipseFinderPage();
        void InitializeComponent();

        winrt::fire_and_forget ComputeButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void GoButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

        Windows::Foundation::DateTime StartTime();
        void StartTime(Windows::Foundation::DateTime const& value);
        Windows::Foundation::DateTime EndTime();
        void EndTime(Windows::Foundation::DateTime const& value);
        bool FindLunar();
        void FindLunar(bool value);
        bool FindSolar();
        void FindSolar(bool value);
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::SearchObjectEntry> AvailableObjects();
        Windows::Foundation::Collections::IObservableVector<CelestiaUWP2::EclipseResult> Eclipses();

        void ObjectChooser_TextChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const&);
        void ObjectChooser_SuggestionChosen(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs const&);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaComponent::CelestiaEclipseFinder finder{ nullptr };
        Windows::Foundation::DateTime startTime;
        Windows::Foundation::DateTime endTime;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::SearchObjectEntry> availableObjects;
        Windows::Foundation::Collections::IObservableVector<CelestiaUWP2::EclipseResult> eclipses;
        bool findLunar{ true };
        bool findSolar{ true };
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> propertyChangedEvent;
        hstring objectPath{ L"" };

        Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<CelestiaUWP2::EclipseResult>> ComputeEclipsesAsync(CelestiaComponent::CelestiaBody const body, CelestiaComponent::CelestiaEclipseKind kind, Windows::Foundation::DateTime const startTime, Windows::Foundation::DateTime const endTime);
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct EclipseResult : EclipseResultT<EclipseResult, implementation::EclipseResult>
    {
    };

    struct EclipseFinderParameter : EclipseFinderParameterT<EclipseFinderParameter, implementation::EclipseFinderParameter>
    {
    };

    struct DateTimeStringConverter : DateTimeStringConverterT<DateTimeStringConverter, implementation::DateTimeStringConverter>
    {
    };

    struct DateTimeReferenceDateTimeConverter : DateTimeReferenceDateTimeConverterT<DateTimeReferenceDateTimeConverter, implementation::DateTimeReferenceDateTimeConverter>
    {
    };

    struct EclipseFinderPage : EclipseFinderPageT<EclipseFinderPage, implementation::EclipseFinderPage>
    {
    };
}
