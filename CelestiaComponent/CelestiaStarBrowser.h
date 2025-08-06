// CelestiaStarBrowser.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <celengine/starbrowser.h>
#include "CelestiaStar.h"
#include "CelestiaStarBrowser.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaStarBrowser : CelestiaStarBrowserT<CelestiaStarBrowser>
    {
        CelestiaStarBrowser(celestia::engine::StarBrowser* sb);
        com_array<CelestiaComponent::CelestiaStar> Stars();

        void Close();
        ~CelestiaStarBrowser();

    private:
        celestia::engine::StarBrowser* sb;
    };
}
