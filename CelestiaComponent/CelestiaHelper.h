//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include "CelestiaHelper.g.h"

namespace CelestiaComponent
{
    struct CelestiaVector;
}

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaHelper : CelestiaHelperT<CelestiaHelper>
    {
        static double JulianDayFromDateTime(Windows::Foundation::DateTime const& dateTime);
        static Windows::Foundation::DateTime DateTimeFromJulianDay(double julianDay);
        static double MinRepresentableJulianDay();
        static double MaxRepresentableJulianDay();
        static Windows::Foundation::DateTime GetMidnight(Windows::Foundation::DateTime const& dateTime);

        static CelestiaComponent::CelestiaVector CelToJ2000Ecliptic(CelestiaComponent::CelestiaVector const& cel);
        static CelestiaComponent::CelestiaVector EclipticToEquatorial(CelestiaComponent::CelestiaVector const& ecliptic);
        static CelestiaComponent::CelestiaVector EquatorialToGalactic(CelestiaComponent::CelestiaVector const& equatorial);
        static CelestiaComponent::CelestiaVector RectToSpherical(CelestiaComponent::CelestiaVector const& rect);
        static double AUToKilometers(double au);
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaHelper : CelestiaHelperT<CelestiaHelper, implementation::CelestiaHelper>
    {
    };
}
