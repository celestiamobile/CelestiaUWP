//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "TimeSettingDialog.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct TimeSettingDialog : TimeSettingDialogT<TimeSettingDialog>
    {
        TimeSettingDialog(double julianDay);
        void InitializeComponent();

        Windows::Foundation::DateTime Date();
        void Date(Windows::Foundation::DateTime const&);

        Windows::Foundation::TimeSpan Time();
        void Time(Windows::Foundation::TimeSpan const&);

        double JulianDay();

        event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

        void CurrentTimeButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void JulianDayInput_TextChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::TextChangedEventArgs const&);
        void TypeSelection_SelectionChanged(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Controls::SelectionChangedEventArgs const&);

        void SetDisplayDate(Windows::Foundation::DateTime const&);

    private:
        event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> propertyChangedEvent;
        Windows::Foundation::DateTime date;
        Windows::Foundation::TimeSpan time;
        double julianDay;
        Windows::Globalization::NumberFormatting::DecimalFormatter numberFormatter { nullptr };

        void ValidateJulianDay();
        void ValidateTime();
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct TimeSettingDialog : TimeSettingDialogT<TimeSettingDialog, implementation::TimeSettingDialog>
    {
    };
}
