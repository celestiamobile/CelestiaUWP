// CelestiaDSOCatalog.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

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
