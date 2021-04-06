//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaBody.h"
#if __has_include("CelestiaBody.g.cpp")
#include "CelestiaBody.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaBody::CelestiaBody(Body* body) : CelestiaBodyT<CelestiaBody, CelestiaAstroObject>(body)
	{
	}

	hstring CelestiaBody::Name()
	{
		return to_hstring(static_cast<Body*>(obj)->getName(true));
	}

	com_array<hstring> CelestiaBody::AlternateSurfaceNames()
	{
		std::vector<std::string>* altSurfaces = static_cast<Body*>(obj)->getAlternateSurfaceNames();
		std::vector<hstring> results;
		if (altSurfaces)
		{
			if (altSurfaces->size() > 0)
			{
				for (unsigned int i = 0; i < altSurfaces->size(); ++i)
				{
					results.push_back(to_hstring((*altSurfaces)[i]));
				}
			}
			delete altSurfaces;
		}
		return com_array<hstring>(results);
	}

	hstring CelestiaBody::InfoURL()
	{
		return to_hstring(static_cast<Body*>(obj)->getInfoURL());
	}
}
