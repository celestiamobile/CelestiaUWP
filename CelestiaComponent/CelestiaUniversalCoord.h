//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include <celengine/univcoord.h>
#include "CelestiaUniversalCoord.g.h"

namespace CelestiaComponent
{
    struct CelestiaVector;
}

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaUniversalCoord : CelestiaUniversalCoordT<CelestiaUniversalCoord>
    {
        CelestiaUniversalCoord(UniversalCoord const& uc);

        double DistanceFrom(CelestiaComponent::CelestiaUniversalCoord const& other);
        CelestiaComponent::CelestiaUniversalCoord DifferenceFrom(CelestiaComponent::CelestiaUniversalCoord const& other);
        CelestiaComponent::CelestiaVector OffsetFrom(CelestiaComponent::CelestiaUniversalCoord const& other);

        static CelestiaComponent::CelestiaUniversalCoord Zero();

        UniversalCoord uc;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaUniversalCoord : CelestiaUniversalCoordT<CelestiaUniversalCoord, implementation::CelestiaUniversalCoord>
    {
    };
}
