//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaDSO.h"
#if __has_include("CelestiaDSO.g.cpp")
#include "CelestiaDSO.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaDSO::CelestiaDSO(DeepSkyObject* dso) : CelestiaDSOT<CelestiaDSO, CelestiaAstroObject>(dso)
	{
	}

	hstring CelestiaDSO::Type()
	{
		return to_hstring(reinterpret_cast<DeepSkyObject*>(obj)->getType());
	}

	hstring CelestiaDSO::InfoURL()
	{
		return to_hstring(static_cast<DeepSkyObject*>(obj)->getInfoURL());
	}
}
