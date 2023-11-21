//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include <celastro/astro.h>
#include "CelestiaDMS.h"
#if __has_include("CelestiaDMS.g.cpp")
#include "CelestiaDMS.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaDMS::CelestiaDMS(int32_t degrees, int32_t minutes, double seconds) : CelestiaDMST<CelestiaDMS>()
    {
        decimal = celestia::astro::degMinSecToDecimal(static_cast<int>(degrees), static_cast<int>(minutes), static_cast<double>(seconds));
    }

    CelestiaDMS::CelestiaDMS(double decimal) : CelestiaDMST<CelestiaDMS>(), decimal(decimal)
    {
    }

    int32_t CelestiaDMS::Degrees()
    {
        int degrees;
        int minutes;
        double seconds;
        celestia::astro::decimalToDegMinSec(decimal, degrees, minutes, seconds);
        return static_cast<int32_t>(degrees);
    }

    int32_t CelestiaDMS::Minutes()
    {
        int degrees;
        int minutes;
        double seconds;
        celestia::astro::decimalToDegMinSec(decimal, degrees, minutes, seconds);
        return static_cast<int32_t>(minutes);
    }

    double CelestiaDMS::Seconds()
    {
        int degrees;
        int minutes;
        double seconds;
        celestia::astro::decimalToDegMinSec(decimal, degrees, minutes, seconds);
        return seconds;
    }

    int32_t CelestiaDMS::HMSHours()
    {
        int hours;
        int minutes;
        double seconds;
        celestia::astro::decimalToHourMinSec(decimal, hours, minutes, seconds);
        return static_cast<int32_t>(hours);
    }

    int32_t CelestiaDMS::HMSMinutes()
    {
        int hours;
        int minutes;
        double seconds;
        celestia::astro::decimalToHourMinSec(decimal, hours, minutes, seconds);
        return static_cast<int32_t>(minutes);
    }

    double CelestiaDMS::HMSSeconds()
    {
        int hours;
        int minutes;
        double seconds;
        celestia::astro::decimalToHourMinSec(decimal, hours, minutes, seconds);
        return seconds;
    }

    double CelestiaDMS::Decimal()
    {
        return decimal;
    }
}
