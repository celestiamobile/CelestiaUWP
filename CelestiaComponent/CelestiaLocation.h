//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include <celengine/location.h>
#include "CelestiaAstroObject.h"
#include "CelestiaLocation.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaLocation : CelestiaLocationT<CelestiaLocation, CelestiaAstroObject>
    {
        CelestiaLocation(Location* location);

        hstring Name();
    };
}
