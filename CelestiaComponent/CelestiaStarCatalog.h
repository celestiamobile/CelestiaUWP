// CelestiaStarCatalog.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

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
