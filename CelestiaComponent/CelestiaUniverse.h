//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include <celengine/universe.h>
#include "CelestiaStar.h"
#include "CelestiaBody.h"
#include "CelestiaBrowserItem.h"
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

        bool IsSelectionMarked(CelestiaComponent::CelestiaSelection const& selection);
        void MarkSelection(CelestiaComponent::CelestiaSelection const& selection, CelestiaComponent::CelestiaMarkerRepresentation marker);
        void UnmarkSelection(CelestiaComponent::CelestiaSelection const& selection);
        void UnmarkAll();

        com_array<CelestiaComponent::CelestiaBrowserItem> ChildrenForStar(CelestiaComponent::CelestiaStar const& star, CelestiaComponent::CelestiaBrowserItemChildrenProvider const& provider);
        com_array<CelestiaComponent::CelestiaBrowserItem> ChildrenForBody(CelestiaComponent::CelestiaBody const& body, CelestiaComponent::CelestiaBrowserItemChildrenProvider const& provider);

        CelestiaComponent::CelestiaStarCatalog StarCatalog();
        CelestiaComponent::CelestiaDSOCatalog DSOCatalog();

        Universe* u;

    private:
        CelestiaComponent::CelestiaStarCatalog starCatalog;
        CelestiaComponent::CelestiaDSOCatalog dsoCatalog;
    };
}
