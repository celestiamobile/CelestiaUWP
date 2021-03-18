#pragma once

#include "CelestiaSelection.h"
#include "CelestiaGotoLocation.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaGotoLocation : CelestiaGotoLocationT<CelestiaGotoLocation>
    {
        CelestiaGotoLocation(CelestiaComponent::CelestiaSelection const& selection, Windows::Foundation::IReference<double> latitude, Windows::Foundation::IReference<double> longitude, Windows::Foundation::IReference<double> distance, Windows::Foundation::IReference<CelestiaComponent::CelestiaGotoLocationDistanceUnit> unit);

        CelestiaComponent::CelestiaSelection selection;
        Windows::Foundation::IReference<double> longitude;
        Windows::Foundation::IReference<double> latitude;
        Windows::Foundation::IReference<double> distance;
        Windows::Foundation::IReference<CelestiaComponent::CelestiaGotoLocationDistanceUnit> unit;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaGotoLocation : CelestiaGotoLocationT<CelestiaGotoLocation, implementation::CelestiaGotoLocation>
    {
    };
}
