// CelestiaGotoLocation.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaGotoLocation.h"
#if __has_include("CelestiaGotoLocation.g.cpp")
#include "CelestiaGotoLocation.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaGotoLocation::CelestiaGotoLocation(CelestiaComponent::CelestiaSelection const& selection, Windows::Foundation::IReference<float> latitude, Windows::Foundation::IReference<float> longitude, Windows::Foundation::IReference<double> distance, Windows::Foundation::IReference<CelestiaComponent::CelestiaGotoLocationDistanceUnit> unit) :
		CelestiaGotoLocationT<CelestiaGotoLocation>(),
		selection(selection),
		latitude(latitude),
		longitude(longitude),
		distance(distance),
		unit(unit)
	{
	}
}
