//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaStar.h"
#include "CelestiaUniversalCoord.h"
#if __has_include("CelestiaStar.g.cpp")
#include "CelestiaStar.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaStar::CelestiaStar(Star* star) : CelestiaStarT<CelestiaStar, CelestiaAstroObject>(star)
	{
	}

	hstring CelestiaStar::InfoURL()
	{
		return to_hstring(static_cast<Star*>(obj)->getInfoURL());
	}

    CelestiaComponent::CelestiaUniversalCoord CelestiaStar::PositionAtTime(double julianDay)
    {
        return make<CelestiaUniversalCoord>(static_cast<Star*>(obj)->getPosition(julianDay));
    }

    hstring CelestiaStar::SpectralType()
    {
        return to_hstring(static_cast<Star*>(obj)->getSpectralType());
    }
}
