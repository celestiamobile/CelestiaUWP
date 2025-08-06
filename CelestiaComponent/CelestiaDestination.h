// CelestiaDestination.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <celestia/destination.h>
#include "CelestiaDestination.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaDestination : CelestiaDestinationT<CelestiaDestination>
    {
        CelestiaDestination(Destination const* d);

        hstring Name();
        hstring Target();
        hstring Content();
        double Distance();

        hstring name;
        hstring target;
        hstring content;
        double distance;
    };
}
