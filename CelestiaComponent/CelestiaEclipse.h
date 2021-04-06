//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include <celestia/eclipsefinder.h>
#include "CelestiaBody.h"
#include "CelestiaEclipse.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaEclipse : CelestiaEclipseT<CelestiaEclipse>
    {
        CelestiaEclipse(Eclipse* e);

        CelestiaComponent::CelestiaBody Occulter();
        CelestiaComponent::CelestiaBody Receiver();
        Windows::Foundation::DateTime StartTime();
        Windows::Foundation::DateTime EndTime();

        CelestiaComponent::CelestiaBody occulter;
        CelestiaComponent::CelestiaBody receiver;

        double startTime;
        double endTime;
    };
}
