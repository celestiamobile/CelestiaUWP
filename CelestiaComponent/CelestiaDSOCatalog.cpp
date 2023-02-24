//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaDSOCatalog.h"
#if __has_include("CelestiaDSOCatalog.g.cpp")
#include "CelestiaDSOCatalog.g.cpp"
#endif

#include "CelestiaGalaxy.h"

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaDSOCatalog::CelestiaDSOCatalog(DSODatabase* db) : CelestiaDSOCatalogT<CelestiaDSOCatalog>(), db(db)
	{
	}

	int32_t CelestiaDSOCatalog::Count()
	{
		return db->size();
	}

	CelestiaComponent::CelestiaDSO CelestiaDSOCatalog::DSOAt(int32_t index)
	{
        auto dso = db->getDSO(index);
        if (to_hstring(dso->getObjTypeName()) == L"galaxy")
            return make<CelestiaGalaxy>(reinterpret_cast<Galaxy *>(dso));
		return make<CelestiaDSO>(dso);
	}

	hstring CelestiaDSOCatalog::DSOName(CelestiaComponent::CelestiaDSO const& DSO)
	{
		return to_hstring(db->getDSOName(reinterpret_cast<DeepSkyObject*>(get_self<CelestiaDSO>(DSO)->obj), true));
	}
}
