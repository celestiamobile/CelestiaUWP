// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "GotoObjectDialog.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct GotoObjectDialog : GotoObjectDialogT<GotoObjectDialog>
    {
        GotoObjectDialog(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);

        hstring ObjectPath();
        float Latitude();
        float Longitude();
        double Distance();
        CelestiaComponent::CelestiaGotoLocationDistanceUnit UnitType();

        Windows::Foundation::Collections::IObservableVector<hstring> Units();
        int32_t Unit();
        void Unit(int32_t);

        void LongitudeText_TextChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::TextChangedEventArgs const&);
        void LatitudeText_TextChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::TextChangedEventArgs const&);
        void DistanceText_TextChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::TextChangedEventArgs const&);
        void ObjectNameText_TextChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const&);
        void ObjectNameText_SuggestionChosen(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs const&);

    private:
        hstring objectPath{ L"" };
        int32_t unit{ 1 };
        Windows::Foundation::IReference<float> latitude{ 0.0f };
        Windows::Foundation::IReference<float> longitude{ 0.0f };
        Windows::Foundation::IReference<double> distance{ 8.0 };
        Windows::Foundation::Collections::IObservableVector<hstring> units;
        CelestiaComponent::CelestiaAppCore appCore{ nullptr };
        CelestiaComponent::CelestiaRenderer renderer{ nullptr };
        Windows::Globalization::NumberFormatting::DecimalFormatter numberFormatter { nullptr };

        void Validate();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct GotoObjectDialog : GotoObjectDialogT<GotoObjectDialog, implementation::GotoObjectDialog>
    {
    };
}
