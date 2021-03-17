#pragma once

#include <celengine/dsodb.h>
#include "CelestiaDSO.h"
#include "CelestiaDSOCatalog.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaDSOCatalog : CelestiaDSOCatalogT<CelestiaDSOCatalog>
    {
        CelestiaDSOCatalog(DSODatabase* db);

        int32_t Count();
        CelestiaComponent::CelestiaDSO DSOAt(int32_t index);

        hstring DSOName(CelestiaComponent::CelestiaDSO const& DSO);

        DSODatabase* db;
    };
}