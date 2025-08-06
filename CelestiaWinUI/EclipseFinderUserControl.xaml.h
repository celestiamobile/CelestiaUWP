// EclipseFinderUserControl.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "EclipseResult.g.h"
#include "DateTimeStringConverter.g.h"
#include "DateTimeReferenceDateTimeConverter.g.h"
#include "EclipseFinderUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
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

    struct EclipseFinderUserControl : EclipseFinderUserControlT<EclipseFinderUserControl>
    {
        EclipseFinderUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        void InitializeComponent();

        winrt::fire_and_forget ComputeButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void GoButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
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
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::EclipseResult> Eclipses();

        void ObjectChooser_TextChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const&);
        void ObjectChooser_SuggestionChosen(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaComponent::CelestiaEclipseFinder finder{ nullptr };
        Windows::Foundation::DateTime startTime;
        Windows::Foundation::DateTime endTime;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::SearchObjectEntry> availableObjects;
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::EclipseResult> eclipses;
        bool findLunar{ true };
        bool findSolar{ true };
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChangedEvent;
        hstring objectPath{ L"" };

        Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<CelestiaWinUI::EclipseResult>> ComputeEclipsesAsync(CelestiaComponent::CelestiaBody const body, CelestiaComponent::CelestiaEclipseKind kind, Windows::Foundation::DateTime const startTime, Windows::Foundation::DateTime const endTime);
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct EclipseResult : EclipseResultT<EclipseResult, implementation::EclipseResult>
    {
    };

    struct DateTimeStringConverter : DateTimeStringConverterT<DateTimeStringConverter, implementation::DateTimeStringConverter>
    {
    };

    struct DateTimeReferenceDateTimeConverter : DateTimeReferenceDateTimeConverterT<DateTimeReferenceDateTimeConverter, implementation::DateTimeReferenceDateTimeConverter>
    {
    };

    struct EclipseFinderUserControl : EclipseFinderUserControlT<EclipseFinderUserControl, implementation::EclipseFinderUserControl>
    {
    };
}
