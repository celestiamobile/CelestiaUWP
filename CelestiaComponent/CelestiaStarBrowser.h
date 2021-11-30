//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include <celengine/starbrowser.h>
#include "CelestiaStar.h"
#include "CelestiaStarBrowser.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaStarBrowser : CelestiaStarBrowserT<CelestiaStarBrowser>
    {
        CelestiaStarBrowser(StarBrowser* sb);
        com_array<CelestiaComponent::CelestiaStar> Stars();

        void Close();
        ~CelestiaStarBrowser();

    private:
        StarBrowser* sb;
    };
}
