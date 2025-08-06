// CelestiaGalaxy.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaGalaxy.h"
#if __has_include("CelestiaGalaxy.g.cpp")
#include "CelestiaGalaxy.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaGalaxy::CelestiaGalaxy(Galaxy* galaxy) : CelestiaGalaxyT<CelestiaGalaxy, CelestiaDSO>(galaxy)
	{
	}

    float CelestiaGalaxy::Radius()
    {
        return static_cast<Galaxy*>(obj)->getRadius();
    }

    float CelestiaGalaxy::Detail()
    {
        return static_cast<Galaxy*>(obj)->getDetail();
    }
}
