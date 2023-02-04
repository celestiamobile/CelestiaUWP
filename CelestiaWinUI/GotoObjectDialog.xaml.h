// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "GotoObjectDialog.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct GotoObjectDialog : GotoObjectDialogT<GotoObjectDialog>
    {
        GotoObjectDialog(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);

        hstring Text();
        float Latitude();
        float Longitude();
        double Distance();
        CelestiaComponent::CelestiaGotoLocationDistanceUnit UnitType();

        hstring LatitudeString();
        hstring LongitudeString();
        hstring DistanceString();
        void LatitudeString(hstring const&);
        void LongitudeString(hstring const&);
        void DistanceString(hstring const&);
        Windows::Foundation::Collections::IObservableVector<hstring> Units();
        int32_t Unit();
        void Unit(int32_t);

        void ObjectNameText_TextChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const&);

    private:
        hstring latitudeString{ L"0" };
        hstring longitudeString{ L"0" };
        hstring distanceString{ L"8" };
        int32_t unit{ 1 };
        Windows::Foundation::Collections::IObservableVector<hstring> units;
        CelestiaComponent::CelestiaAppCore appCore{ nullptr };
        CelestiaComponent::CelestiaRenderer renderer{ nullptr };
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct GotoObjectDialog : GotoObjectDialogT<GotoObjectDialog, implementation::GotoObjectDialog>
    {
    };
}
