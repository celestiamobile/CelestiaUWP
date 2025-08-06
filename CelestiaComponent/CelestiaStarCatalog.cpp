// CelestiaStarCatalog.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaStarCatalog.h"
#if __has_include("CelestiaStarCatalog.g.cpp")
#include "CelestiaStarCatalog.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaStarCatalog::CelestiaStarCatalog(StarDatabase* db) : CelestiaStarCatalogT<CelestiaStarCatalog>(), db(db)
	{
	}

	int32_t CelestiaStarCatalog::Count()
	{
		return db->size();
	}

	CelestiaComponent::CelestiaStar CelestiaStarCatalog::StarAt(int32_t index)
	{
		return make<CelestiaStar>(db->getStar(index));
	}

	hstring CelestiaStarCatalog::StarName(CelestiaComponent::CelestiaStar const& star)
	{
		return to_hstring(db->getStarName(*reinterpret_cast<Star*>(get_self<CelestiaStar>(star)->obj), true));
	}
}
