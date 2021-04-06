//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include "CelestiaSelection.h"
#include "CelestiaGotoLocation.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaGotoLocation : CelestiaGotoLocationT<CelestiaGotoLocation>
    {
        CelestiaGotoLocation(CelestiaComponent::CelestiaSelection const& selection, Windows::Foundation::IReference<float> latitude, Windows::Foundation::IReference<float> longitude, Windows::Foundation::IReference<double> distance, Windows::Foundation::IReference<CelestiaComponent::CelestiaGotoLocationDistanceUnit> unit);

        CelestiaComponent::CelestiaSelection selection;
        Windows::Foundation::IReference<float> longitude;
        Windows::Foundation::IReference<float> latitude;
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
