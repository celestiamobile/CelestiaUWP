// CelestiaDSO.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <celengine/deepskyobj.h>
#include "CelestiaAstroObject.h"
#include "CelestiaDSO.g.h"

namespace CelestiaComponent
{
    struct CelestiaVector;
}

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaDSO : CelestiaDSOT<CelestiaDSO, CelestiaAstroObject>
    {
        CelestiaDSO(DeepSkyObject* dso);
        CelestiaComponent::CelestiaDSOType ObjectType() const;
        hstring Type();
        hstring InfoURL();
        CelestiaComponent::CelestiaVector Position();
        hstring Description();
    };
}
