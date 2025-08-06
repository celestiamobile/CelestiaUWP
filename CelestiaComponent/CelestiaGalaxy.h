// CelestiaGalaxy.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <celengine/galaxy.h>
#include "CelestiaDSO.h"
#include "CelestiaGalaxy.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaGalaxy : CelestiaGalaxyT<CelestiaGalaxy, CelestiaDSO>
    {
        CelestiaGalaxy(Galaxy* galaxy);
        float Radius();
        float Detail();
    };
}
