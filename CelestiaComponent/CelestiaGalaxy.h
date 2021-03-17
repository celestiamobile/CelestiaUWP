#pragma once

#include <celengine/galaxy.h>
#include "CelestiaDSO.h"
#include "CelestiaGalaxy.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaGalaxy : CelestiaGalaxyT<CelestiaGalaxy, CelestiaDSO>
    {
        CelestiaGalaxy(Galaxy* galaxy);
    };
}