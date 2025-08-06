// CelestiaDSO.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaDSO.h"
#include "CelestiaVector.h"
#if __has_include("CelestiaDSO.g.cpp")
#include "CelestiaDSO.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaDSO::CelestiaDSO(DeepSkyObject* dso) : CelestiaDSOT<CelestiaDSO, CelestiaAstroObject>(dso)
	{
	}

    CelestiaComponent::CelestiaDSOType CelestiaDSO::ObjectType() const
    {
        return static_cast<CelestiaComponent::CelestiaDSOType>(reinterpret_cast<DeepSkyObject*>(obj)->getObjType());
    }

	hstring CelestiaDSO::Type()
	{
		return to_hstring(reinterpret_cast<DeepSkyObject*>(obj)->getType());
	}

	hstring CelestiaDSO::InfoURL()
	{
		return to_hstring(static_cast<DeepSkyObject*>(obj)->getInfoURL());
	}

    CelestiaComponent::CelestiaVector CelestiaDSO::Position()
    {
        auto pos = static_cast<DeepSkyObject*>(obj)->getPosition();
        return make<CelestiaVector>(pos.x(), pos.y(), pos.z());
    }

    hstring CelestiaDSO::Description()
    {
        return to_hstring(static_cast<DeepSkyObject*>(obj)->getDescription());
    }
}
