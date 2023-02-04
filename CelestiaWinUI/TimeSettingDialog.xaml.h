#pragma once

#include "TimeSettingDialog.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct TimeSettingDialog : TimeSettingDialogT<TimeSettingDialog>
    {
        TimeSettingDialog(double julianDay);

        Windows::Foundation::DateTime Date();
        void Date(Windows::Foundation::DateTime const&);

        Windows::Foundation::TimeSpan Time();
        void Time(Windows::Foundation::TimeSpan const&);

        double JulianDay();

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

        void CurrentTimeButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void JulianDayInput_TextChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::TextChangedEventArgs const&);
        void TypeSelection_SelectionChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&);

        void SetDisplayDate(Windows::Foundation::DateTime const&);

    private:
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChangedEvent;
        Windows::Foundation::DateTime date;
        Windows::Foundation::TimeSpan time;
        double julianDay;
        Windows::Globalization::NumberFormatting::DecimalFormatter numberFormatter { nullptr };

        void ValidateJulianDay();
        void ValidateTime();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct TimeSettingDialog : TimeSettingDialogT<TimeSettingDialog, implementation::TimeSettingDialog>
    {
    };
}
