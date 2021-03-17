#pragma once

#include <celengine/universe.h>
#include "CelestiaSelection.h"
#include "CelestiaUniverse.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaUniverse : CelestiaUniverseT<CelestiaUniverse>
    {
        CelestiaUniverse(Universe* u);

        CelestiaComponent::CelestiaSelection Find(hstring const& name);

        Universe* u;
    };
}