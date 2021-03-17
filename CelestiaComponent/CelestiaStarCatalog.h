#pragma once

#include <celengine/stardb.h>
#include "CelestiaStar.h"
#include "CelestiaStarCatalog.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaStarCatalog : CelestiaStarCatalogT<CelestiaStarCatalog>
    {
        CelestiaStarCatalog(StarDatabase* db);

        int32_t Count();
        CelestiaComponent::CelestiaStar StarAt(int32_t index);

        hstring StarName(CelestiaComponent::CelestiaStar const& star);

        StarDatabase* db;
    };
}