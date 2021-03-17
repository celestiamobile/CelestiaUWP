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
