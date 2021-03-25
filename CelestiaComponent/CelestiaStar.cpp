#include "pch.h"
#include "CelestiaStar.h"
#if __has_include("CelestiaStar.g.cpp")
#include "CelestiaStar.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaStar::CelestiaStar(Star* star) : CelestiaStarT<CelestiaStar, CelestiaAstroObject>(star)
	{
	}

	hstring CelestiaStar::InfoURL()
	{
		return to_hstring(static_cast<Star*>(obj)->getInfoURL());
	}
}
