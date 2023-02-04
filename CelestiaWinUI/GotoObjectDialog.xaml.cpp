#include "pch.h"
#include "GotoObjectDialog.xaml.h"
#if __has_include("GotoObjectDialog.g.cpp")
#include "GotoObjectDialog.g.cpp"
#endif

#include <charconv>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    GotoObjectDialog::GotoObjectDialog(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer)
    {
        units = single_threaded_observable_vector<hstring>();
        std::vector<hstring> unitVector{ LocalizationHelper::Localize(L"km"), LocalizationHelper::Localize(L"radii"), LocalizationHelper::Localize(L"au") };
        units.ReplaceAll(unitVector);
        InitializeComponent();
        Title(box_value(LocalizationHelper::Localize(L"Go to Object")));
        PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        SecondaryButtonText(LocalizationHelper::Localize(L"Cancel"));
        ObjectNameText().PlaceholderText(LocalizationHelper::Localize(L"Object Name"));
        LongitudeText().PlaceholderText(LocalizationHelper::Localize(L"Longitude"));
        LatitudeText().PlaceholderText(LocalizationHelper::Localize(L"Latitude"));
        DistanceText().PlaceholderText(LocalizationHelper::Localize(L"Distance"));
    }

    float GotoObjectDialog::Latitude()
    {
        auto str = winrt::to_string(latitudeString);
        float value = 0.0f;
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
        if (ec == std::errc {} && ptr == str.data() + str.size())
            return value;
        return 0;
    }

    float GotoObjectDialog::Longitude()
    {
        auto str = winrt::to_string(longitudeString);
        float value = 0.0f;
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
        if (ec == std::errc{} && ptr == str.data() + str.size())
            return value;
        return 0;
    }

    double GotoObjectDialog::Distance()
    {
        auto str = winrt::to_string(distanceString);
        double value = 0.0;
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);
        if (ec == std::errc{} && ptr == str.data() + str.size())
            return value;
        return 0;
    }

    CelestiaComponent::CelestiaGotoLocationDistanceUnit GotoObjectDialog::UnitType()
    {
        return (CelestiaComponent::CelestiaGotoLocationDistanceUnit)unit;
    }

    hstring GotoObjectDialog::Text()
    {
        return ObjectNameText().Text();
    }

    hstring GotoObjectDialog::LatitudeString()
    {
        return latitudeString;
    }

    hstring GotoObjectDialog::LongitudeString()
    {
        return longitudeString;

    }

    hstring GotoObjectDialog::DistanceString()
    {
        return distanceString;
    }

    void GotoObjectDialog::LatitudeString(hstring const& value)
    {
        latitudeString = value;
    }

    void GotoObjectDialog::LongitudeString(hstring const& value)
    {
        longitudeString = value;
    }

    void GotoObjectDialog::DistanceString(hstring const& value)
    {
        distanceString = value;
    }

    Collections::IObservableVector<hstring> GotoObjectDialog::Units()
    {
        return units;
    }

    int32_t GotoObjectDialog::Unit()
    {
        return unit;
    }

    void GotoObjectDialog::Unit(int32_t value)
    {
        unit = value;
    }

    void GotoObjectDialog::ObjectNameText_TextChanged(IInspectable const& sender, Controls::AutoSuggestBoxTextChangedEventArgs const& args)
    {
        if (args.Reason() != Controls::AutoSuggestionBoxTextChangeReason::UserInput) return;

        auto suggestBox = sender.as<Controls::AutoSuggestBox>();
        auto text = suggestBox.Text();
        if (text.empty())
        {
            suggestBox.ItemsSource(single_threaded_observable_vector<hstring>());
            return;
        }

        renderer.EnqueueTask([weak_this{ get_weak() }, suggestBox, text]()
            {
                auto strong_this = weak_this.get();
                if (!strong_this) return;
                auto comCompletions = strong_this->appCore.Simulation().GetCompletion(text);
                std::vector<hstring> completions{ comCompletions.begin(), comCompletions.end() };
                strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [text, completions, suggestBox]()
                    {
                        if (suggestBox.Text() == text)
                        {
                            auto source = single_threaded_observable_vector<hstring>();
                            source.ReplaceAll(completions);
                            suggestBox.ItemsSource(source);
                        }
                    });
            });
    }
}
