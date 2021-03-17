#pragma once

#include <celengine/universe.h>
#include "CelestiaSelection.h"
#include "CelestiaStarCatalog.h"
#include "CelestiaDSOCatalog.h"
#include "CelestiaUniverse.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaUniverse : CelestiaUniverseT<CelestiaUniverse>
    {
        CelestiaUniverse(Universe* u);

        CelestiaComponent::CelestiaSelection Find(hstring const& name);
        hstring NameForSelection(CelestiaComponent::CelestiaSelection const& selection);

        CelestiaComponent::CelestiaStarCatalog StarCatalog();
        CelestiaComponent::CelestiaDSOCatalog DSOCatalog();

        Universe* u;

    private:
        CelestiaComponent::CelestiaStarCatalog starCatalog;
        CelestiaComponent::CelestiaDSOCatalog dsoCatalog;
    };
}