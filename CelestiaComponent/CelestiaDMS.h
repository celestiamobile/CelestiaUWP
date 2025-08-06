// CelestiaDMS.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <vector>
#include "CelestiaDMS.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaDMS : CelestiaDMST<CelestiaDMS>
    {
        CelestiaDMS(int32_t degrees, int32_t minutes, double seconds);
        CelestiaDMS(double decimal);

        int32_t Degrees();
        int32_t Minutes();
        double Seconds();
        int32_t HMSHours();
        int32_t HMSMinutes();
        double HMSSeconds();
        double Decimal();
    private:
        double decimal;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaDMS : CelestiaDMST<CelestiaDMS, implementation::CelestiaDMS>
    {
    };
}
