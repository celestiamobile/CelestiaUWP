// GotoObjectDialog.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

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
using namespace Windows::Globalization::NumberFormatting;

namespace winrt::CelestiaWinUI::implementation
{
    GotoObjectDialog::GotoObjectDialog(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer)
    {
        object = CelestiaComponent::CelestiaSelection();
        numberFormatter = DecimalFormatter();
        numberFormatter.FractionDigits(0);
        numberFormatter.IsGrouped(false);
        units = single_threaded_observable_vector<hstring>();
        std::vector<hstring> unitVector{ LocalizationHelper::Localize(L"km", L"Unit"), LocalizationHelper::Localize(L"radii", L"In Go to, specify the distance based on the object radius"), LocalizationHelper::Localize(L"au", L"Astronomical unit") };
        units.ReplaceAll(unitVector);
    }

    void GotoObjectDialog::InitializeComponent()
    {
        GotoObjectDialogT::InitializeComponent();
        LongitudeText().Text(numberFormatter.FormatDouble(static_cast<double>(Longitude())));
        LatitudeText().Text(numberFormatter.FormatDouble(static_cast<double>(Latitude())));
        DistanceText().Text(numberFormatter.FormatDouble(Distance()));
        Title(box_value(LocalizationHelper::Localize(L"Go to Object", L"")));
        PrimaryButtonText(LocalizationHelper::Localize(L"OK", L""));
        SecondaryButtonText(LocalizationHelper::Localize(L"Cancel", L""));
        ObjectNameText().Header(box_value(LocalizationHelper::Localize(L"Object Name", L"")));
        LongitudeText().Header(box_value(LocalizationHelper::Localize(L"Longitude", L"Coordinates")));
        LatitudeText().Header(box_value(LocalizationHelper::Localize(L"Latitude", L"Coordinates")));
        DistanceText().Header(box_value(LocalizationHelper::Localize(L"Distance", L"Distance to the object (in Go to)")));
    }

    float GotoObjectDialog::Latitude()
    {
        return latitude != nullptr ? latitude.Value() : 0.0f;
    }

    float GotoObjectDialog::Longitude()
    {
        return longitude != nullptr ? longitude.Value() : 0.0f;
    }

    double GotoObjectDialog::Distance()
    {
        return distance != nullptr ? distance.Value() : 8.0;
    }

    CelestiaComponent::CelestiaGotoLocationDistanceUnit GotoObjectDialog::UnitType()
    {
        return (CelestiaComponent::CelestiaGotoLocationDistanceUnit)unit;
    }

    CelestiaComponent::CelestiaSelection GotoObjectDialog::Object()
    {
        return object;
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

    void GotoObjectDialog::LongitudeText_TextChanged(IInspectable const&, Controls::TextChangedEventArgs const&)
    {
        auto longitudeValue = numberFormatter.ParseDouble(LongitudeText().Text());
        if (longitudeValue != nullptr)
        {
            auto floatValue = static_cast<float>(longitudeValue.Value());
            if (floatValue >= -180.0f && floatValue <= 180.0f)
                longitude = floatValue;
            else
                longitude = nullptr;
        }
        else
        {
            longitude = nullptr;
        }
        Validate();
    }

    void GotoObjectDialog::LatitudeText_TextChanged(IInspectable const&, Controls::TextChangedEventArgs const&)
    {
        auto latitudeValue = numberFormatter.ParseDouble(LatitudeText().Text());
        if (latitudeValue != nullptr)
        {
            auto floatValue = static_cast<float>(latitudeValue.Value());
            if (floatValue >= -90.0f && floatValue <= 90.0f)
                latitude = floatValue;
            else
                latitude = nullptr;
        }
        else
        {
            latitude = nullptr;
        }
        Validate();
    }

    void GotoObjectDialog::DistanceText_TextChanged(IInspectable const&, Controls::TextChangedEventArgs const&)
    {
        auto distanceValue = numberFormatter.ParseDouble(DistanceText().Text());
        if (distanceValue != nullptr && distanceValue.Value() >= 0.0)
        {
            distance = distanceValue.Value();
        }
        else
        {
            distance = nullptr;
        }
        Validate();
    }

    void GotoObjectDialog::ObjectNameText_TextChanged(IInspectable const& sender, Controls::AutoSuggestBoxTextChangedEventArgs const& args)
    {
        auto suggestBox = sender.as<Controls::AutoSuggestBox>();
        auto text = suggestBox.Text();
        if (args.Reason() != Controls::AutoSuggestionBoxTextChangeReason::UserInput) return;

        if (text.empty())
        {
            object = CelestiaComponent::CelestiaSelection();
            suggestBox.ItemsSource(single_threaded_observable_vector<CelestiaComponent::CelestiaCompletion>());
            return;
        }

        renderer.EnqueueTask([weak_this{ get_weak() }, suggestBox, text]()
            {
                auto strong_this = weak_this.get();
                if (!strong_this) return;
                auto comCompletions = strong_this->appCore.Simulation().GetCompletion(text);
                auto fullMatch = strong_this->appCore.Simulation().Find(text);
                auto source = single_threaded_observable_vector<CelestiaComponent::CelestiaCompletion>();
                for (auto const& completion : comCompletions)
                    source.Append(completion);
                strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [text, source, suggestBox, fullMatch, strong_this]()
                    {
                        if (suggestBox.Text() == text)
                        {
                            strong_this->object = fullMatch;
                            suggestBox.ItemsSource(source);
                        }
                    });
            });
    }

    void GotoObjectDialog::ObjectNameText_SuggestionChosen(IInspectable const& sender, Controls::AutoSuggestBoxSuggestionChosenEventArgs const& args)
    {
        auto suggestBox = sender.as<Controls::AutoSuggestBox>();
        auto selected = args.SelectedItem().as<CelestiaComponent::CelestiaCompletion>();
        object = selected.Selection();
        suggestBox.Text(selected.Name());
    }

    void GotoObjectDialog::Validate()
    {
        if (longitude == nullptr || latitude == nullptr || distance == nullptr)
            IsPrimaryButtonEnabled(false);
        else
            IsPrimaryButtonEnabled(true);
    }
}
