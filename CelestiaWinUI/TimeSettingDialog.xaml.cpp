// TimeSettingDialog.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "TimeSettingDialog.xaml.h"
#if __has_include("TimeSettingDialog.g.cpp")
#include "TimeSettingDialog.g.cpp"
#endif

#include <fmt/printf.h>
#include <fmt/xchar.h>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation;
using namespace Windows::Globalization::NumberFormatting;

namespace winrt::CelestiaWinUI::implementation
{
    TimeSettingDialog::TimeSettingDialog(double julianDay) : julianDay(julianDay)
    {
        numberFormatter = DecimalFormatter();
        numberFormatter.FractionDigits(0);
        numberFormatter.IsGrouped(false);
    }

    void TimeSettingDialog::InitializeComponent()
    {
        TimeSettingDialogT::InitializeComponent();

        auto types = single_threaded_observable_vector<hstring>({ LocalizationHelper::Localize(L"Picker", L"Select time via picker"), LocalizationHelper::Localize(L"Julian Day", L"Select time via entering Julian day") });
        TypeSelection().ItemsSource(types);

        // 9999/12/30 23:59:59 GMT
        DateTime maxTime{ std::chrono::seconds{253402214399LL} };
        // 1/1/2 00:00:00 GMT
        DateTime minTime{ std::chrono::seconds{-2177366400LL} };

        DatePicker().MaxYear(maxTime);
        DatePicker().MinYear(minTime);
        Title(box_value(LocalizationHelper::Localize(L"Set Time", L"Button to confirm selected simulation time")));
        PrimaryButtonText(LocalizationHelper::Localize(L"OK", L""));
        SecondaryButtonText(LocalizationHelper::Localize(L"Cancel", L""));
        CurrentTimeButton().Content(box_value(LocalizationHelper::Localize(L"Set to Current Time", L"Set simulation time to device")));
        JulianDayInput().Text(numberFormatter.Format(std::round(julianDay * 10000.0) / 10000.0));
        if (julianDay < CelestiaHelper::MinRepresentableJulianDay() || julianDay > CelestiaHelper::MaxRepresentableJulianDay())
        {
            SetDisplayDate(clock::now());
            TypeSelection().SelectedIndex(1);
        }
        else
        {
            SetDisplayDate(CelestiaHelper::DateTimeFromJulianDay(julianDay));
            TypeSelection().SelectedIndex(0);
        }
    }

    DateTime TimeSettingDialog::Date()
    {
        return date;
    }

    void TimeSettingDialog::Date(DateTime const& value)
    {
        date = value;
        ValidateTime();
    }

    TimeSpan TimeSettingDialog::Time()
    {
        return time;
    }

    void TimeSettingDialog::Time(TimeSpan const& value)
    {
        time = value;
        ValidateTime();
    }

    double TimeSettingDialog::JulianDay()
    {
        auto selectedIndex = TypeSelection().SelectedIndex();
        if (selectedIndex == 0)
            return CelestiaHelper::JulianDayFromDateTime(Date() + Time());
        else if (selectedIndex == 1)
            return julianDay;
        return 0;
    }

    void TimeSettingDialog::CurrentTimeButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        SetDisplayDate(clock::now());
    }

    void TimeSettingDialog::JulianDayInput_TextChanged(IInspectable const&, Controls::TextChangedEventArgs const&)
    {
        ValidateJulianDay();
    }

    void TimeSettingDialog::TypeSelection_SelectionChanged(IInspectable const&, Controls::SelectionChangedEventArgs const&)
    {
        auto selectedIndex = TypeSelection().SelectedIndex();
        if (selectedIndex == 0)
        {
            PickerPanel().Visibility(Visibility::Visible);
            JulianDayPanel().Visibility(Visibility::Collapsed);
            ValidateTime();
        }
        else if (selectedIndex == 1)
        {
            PickerPanel().Visibility(Visibility::Collapsed);
            JulianDayPanel().Visibility(Visibility::Visible);
            ValidateJulianDay();
        }
        else
        {
            PickerPanel().Visibility(Visibility::Collapsed);
            JulianDayPanel().Visibility(Visibility::Collapsed);
        }
    }

    event_token TimeSettingDialog::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChangedEvent.add(handler);
    }

    void TimeSettingDialog::PropertyChanged(event_token const& token) noexcept
    {
        propertyChangedEvent.remove(token);
    }

    void TimeSettingDialog::SetDisplayDate(DateTime const& value)
    {
        try
        {
            auto midnight = CelestiaHelper::GetMidnight(value);
            Date(midnight);
            auto span = value - midnight;
            Time(span);

            propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"Date"));
            propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"Time"));
        }
        catch (hresult_error const&)
        {
            SetDisplayDate(clock::now());
        }
    }

    void TimeSettingDialog::ValidateJulianDay()
    {
        if (TypeSelection().SelectedIndex() != 1) return;
        auto value = numberFormatter.ParseDouble(JulianDayInput().Text());
        if (value != nullptr)
        {
            julianDay = value.Value();
            IsPrimaryButtonEnabled(true);
            JulianDayErrorText().Visibility(Visibility::Collapsed);
        }
        else
        {
            IsPrimaryButtonEnabled(false);
            JulianDayErrorText().Visibility(Visibility::Visible);
            JulianDayErrorText().Text(LocalizationHelper::Localize(L"Invalid Julian day string.", L"The input of julian day is not valid"));
        }
    }

    void TimeSettingDialog::ValidateTime()
    {
        if (TypeSelection().SelectedIndex() != 0) return;
        try
        {
            auto combinedTime = Date() + Time();
            IsPrimaryButtonEnabled(true);
            ErrorText().Visibility(Visibility::Collapsed);
        }
        catch (hresult_error const&)
        {
            IsPrimaryButtonEnabled(false);
            ErrorText().Visibility(Visibility::Visible);
            ErrorText().Text(LocalizationHelper::Localize(L"Selected time is out of range.", L"Error message, the time in the picker is considered out of range by the system"));
        }
    }
}