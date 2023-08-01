//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include "CelestiaBody.h"
#include "CelestiaEclipse.h"
#include "CelestiaEclipseFinder.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaEclipseFinder : CelestiaEclipseFinderT<CelestiaEclipseFinder>
    {
        CelestiaEclipseFinder(CelestiaComponent::CelestiaBody body);
        com_array<CelestiaComponent::CelestiaEclipse> Search(CelestiaComponent::CelestiaEclipseKind kind, double startJulianDay, double endJulianDay);
        ~CelestiaEclipseFinder();

        void Abort();
        void Close();

        EclipseFinder* f;
        EclipseFinderWatcher* w;
        bool aborted;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaEclipseFinder : CelestiaEclipseFinderT<CelestiaEclipseFinder, implementation::CelestiaEclipseFinder>
    {
    };
}
