//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include <celengine/body.h>
#include "CelestiaAstroObject.h"
#include "CelestiaBody.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaBody : CelestiaBodyT<CelestiaBody, CelestiaAstroObject>
    {
        CelestiaBody(Body* body);

        hstring Name();
        com_array<hstring> AlternateSurfaceNames();
        hstring InfoURL();
    };
}
