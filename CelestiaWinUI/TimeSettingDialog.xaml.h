#pragma once

#include "TimeSettingDialog.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct TimeSettingDialog : TimeSettingDialogT<TimeSettingDialog>
    {
        TimeSettingDialog(Windows::Foundation::DateTime const& original);

        Windows::Foundation::DateTime Date();
        void Date(Windows::Foundation::DateTime const&);

        Windows::Foundation::TimeSpan Time();
        void Time(Windows::Foundation::TimeSpan const&);

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

        void CurrentTimeButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);

        Windows::Foundation::DateTime DisplayDate();
        void DisplayDate(Windows::Foundation::DateTime const&);

    private:
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChangedEvent;
        Windows::Foundation::DateTime date;
        Windows::Foundation::TimeSpan time;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct TimeSettingDialog : TimeSettingDialogT<TimeSettingDialog, implementation::TimeSettingDialog>
    {
    };
}