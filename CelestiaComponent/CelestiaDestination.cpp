//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaDestination.h"
#if __has_include("CelestiaDestination.g.cpp")
#include "CelestiaDestination.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaDestination::CelestiaDestination(Destination const* d) : CelestiaDestinationT<CelestiaDestination>(), name(to_hstring(d->name)), target(to_hstring(d->target)), content(to_hstring(d->description)), distance(d->distance)
	{
	}

	hstring CelestiaDestination::Name()
	{
		return name;
	}

	hstring CelestiaDestination::Target()
	{
		return target;
	}

	hstring CelestiaDestination::Content()
	{
		return content;
	}

	double CelestiaDestination::Distance()
	{
		return distance;
	}
}
