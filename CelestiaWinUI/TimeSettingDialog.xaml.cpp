#include "pch.h"
#include "TimeSettingDialog.xaml.h"
#if __has_include("TimeSettingDialog.g.cpp")
#include "TimeSettingDialog.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    TimeSettingDialog::TimeSettingDialog(DateTime const& original)
    {
        InitializeComponent();

        // 9999/12/30 11:59:59 GMT
        DateTime maxTime{ std::chrono::seconds{253402214399LL} };
        // 1/1/2 00:00:00 GMT
        DateTime minTime{ std::chrono::seconds{-2177366400LL} };

        DatePicker().MaxYear(maxTime);
        DatePicker().MinYear(minTime);
        Title(box_value(LocalizationHelper::Localize(L"Set Time")));
        PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        SecondaryButtonText(LocalizationHelper::Localize(L"Cancel"));
        CurrentTimeButton().Content(box_value(LocalizationHelper::Localize(L"Set to Current Time")));
        DisplayDate(original);
    }

    DateTime TimeSettingDialog::Date()
    {
        return date;
    }

    void TimeSettingDialog::Date(DateTime const& value)
    {
        date = value;
    }

    TimeSpan TimeSettingDialog::Time()
    {
        return time;
    }

    void TimeSettingDialog::Time(TimeSpan const& value)
    {
        time = value;
    }

    void TimeSettingDialog::CurrentTimeButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        DisplayDate(clock::now());
    }

    event_token TimeSettingDialog::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChangedEvent.add(handler);
    }

    void TimeSettingDialog::PropertyChanged(event_token const& token) noexcept
    {
        propertyChangedEvent.remove(token);
    }

    DateTime TimeSettingDialog::DisplayDate()
    {
        return Date() + Time();
    }

    void TimeSettingDialog::DisplayDate(DateTime const& value)
    {
        auto timeT = clock::to_time_t(value);
        tm localTime;
        auto err = localtime_s(&localTime, &timeT);
        if (err)
            return;

        auto localMidnight = localTime;
        localMidnight.tm_hour = 0;
        localMidnight.tm_min = 0;
        localMidnight.tm_sec = 0;
        auto midnightTimeT = mktime(&localMidnight);
        auto midnight = clock::from_time_t(midnightTimeT);
        Date(midnight);
        auto span = value - midnight;
        Time(span);
        propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"Date"));
        propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"Time"));
    }
}