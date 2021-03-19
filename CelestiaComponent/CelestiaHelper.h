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
