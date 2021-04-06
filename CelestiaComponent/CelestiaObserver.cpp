//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaObserver.h"
#if __has_include("CelestiaObserver.g.cpp")
#include "CelestiaObserver.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaObserver::CelestiaObserver(Observer* observer) : CelestiaObserverT<CelestiaObserver>(), o(observer)
	{
	}

	void CelestiaObserver::DisplayedSurfaceName(hstring const& displayedSurfaceName)
	{
		o->setDisplayedSurface(to_string(displayedSurfaceName));
	}

	hstring CelestiaObserver::DisplayedSurfaceName()
	{
		return to_hstring(o->getDisplayedSurface());
	}
}
