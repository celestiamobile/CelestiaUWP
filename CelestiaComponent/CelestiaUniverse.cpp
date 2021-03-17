#include "pch.h"
#include "CelestiaUniverse.h"
#if __has_include("CelestiaUniverse.g.cpp")
#include "CelestiaUniverse.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaUniverse::CelestiaUniverse(Universe* u) : CelestiaUniverseT<CelestiaUniverse>(), u(u), starCatalog(nullptr), dsoCatalog(nullptr)
	{
	}

	CelestiaComponent::CelestiaSelection CelestiaUniverse::Find(hstring const& name)
	{
		return make<CelestiaSelection>(u->find(to_string(name)));
	}

	hstring CelestiaUniverse::NameForSelection(CelestiaComponent::CelestiaSelection const& selection)
	{
		Selection *s = get_self<CelestiaSelection>(selection)->s;
		switch (s->getType())
		{
		case Selection::Type_Star:
			return get_self<CelestiaStarCatalog>(StarCatalog())->StarName(make<CelestiaStar>(s->star()));
		case Selection::Type_Body:
			return to_hstring(reinterpret_cast<Body*>(s->object())->getName(true));
		case Selection::Type_DeepSky:
			return get_self<CelestiaDSOCatalog>(DSOCatalog())->DSOName(make<CelestiaDSO>(s->deepsky()));
		case Selection::Type_Location:
			return to_hstring(s->location()->getName(true));
		default:
			return L"";
		}
	}

	CelestiaComponent::CelestiaStarCatalog CelestiaUniverse::StarCatalog()
	{
		if (starCatalog == nullptr)
			starCatalog = make<CelestiaStarCatalog>(u->getStarCatalog());
		return starCatalog;
	}

	CelestiaComponent::CelestiaDSOCatalog CelestiaUniverse::DSOCatalog()
	{
		if (dsoCatalog == nullptr)
			dsoCatalog = make<CelestiaDSOCatalog>(u->getDSOCatalog());
		return dsoCatalog;
	}
}
