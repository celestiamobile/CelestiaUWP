//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include <celephem/orbit.h>
#include "CelestiaOrbit.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaOrbit : CelestiaOrbitT<CelestiaOrbit>
    {
        CelestiaOrbit(Orbit* o);

        bool IsPeriodic();
        double Period();
        double BoundingRadius();
        double ValidBeginTime();
        double ValidEndTime();

        CelestiaComponent::CelestiaVector VelocityAtTime(Windows::Foundation::DateTime const& time);
        CelestiaComponent::CelestiaVector PositionAtTime(Windows::Foundation::DateTime const& time);

        Orbit* o;
    };
}
