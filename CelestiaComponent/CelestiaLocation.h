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