//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include <celengine/astro.h>
#include "CelestiaDMS.h"
#if __has_include("CelestiaDMS.g.cpp")
#include "CelestiaDMS.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaDMS::CelestiaDMS(int32_t degrees, int32_t minutes, double seconds) : CelestiaDMST<CelestiaDMS>(), degrees(degrees), minutes(minutes), seconds(seconds)
    {
    }

    CelestiaDMS::CelestiaDMS(double decimal) : CelestiaDMST<CelestiaDMS>()
    {
        int degrees, minutes;
        double seconds;
        astro::decimalToDegMinSec(decimal, degrees, minutes, seconds);
        this->degrees = degrees;
        this->minutes = minutes;
        this->seconds = seconds;
    }

    int32_t CelestiaDMS::Degrees()
    {
        return degrees;
    }

    int32_t CelestiaDMS::Hours()
    {
        return degrees;
    }

    int32_t CelestiaDMS::Minutes()
    {
        return minutes;
    }

    double CelestiaDMS::Seconds()
    {
        return seconds;
    }

    double CelestiaDMS::Decimal()
    {
        return astro::degMinSecToDecimal(degrees, minutes, seconds);
    }
}
