//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include "CelestiaHelper.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaHelper : CelestiaHelperT<CelestiaHelper>
    {
        static double JulianDayFromDateTime(Windows::Foundation::DateTime const& dateTime);
        static Windows::Foundation::DateTime DateTimeFromJulianDay(double julianDay);
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaHelper : CelestiaHelperT<CelestiaHelper, implementation::CelestiaHelper>
    {
    };
}
