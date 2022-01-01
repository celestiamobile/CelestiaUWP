//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaUniversalCoord.h"
#include "CelestiaVector.h"
#if __has_include("CelestiaUniversalCoord.g.cpp")
#include "CelestiaUniversalCoord.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaUniversalCoord::CelestiaUniversalCoord(UniversalCoord const& uc) : CelestiaUniversalCoordT<CelestiaUniversalCoord>(), uc(uc)
    {
    }

    double CelestiaUniversalCoord::CelestiaUniversalCoord::DistanceFrom(CelestiaComponent::CelestiaUniversalCoord const& other)
    {
        return uc.distanceFromKm(get_self<CelestiaUniversalCoord>(other)->uc);
    }

    CelestiaComponent::CelestiaVector CelestiaUniversalCoord::OffsetFrom(CelestiaComponent::CelestiaUniversalCoord const& other)
    {
        auto offset = uc.offsetFromKm(get_self<CelestiaUniversalCoord>(other)->uc);
        return make<CelestiaVector>(offset.x(), offset.y(), offset.z());
    }

    CelestiaComponent::CelestiaUniversalCoord CelestiaUniversalCoord::Zero()
    {
        return make<CelestiaUniversalCoord>(UniversalCoord::Zero());
    }
}
