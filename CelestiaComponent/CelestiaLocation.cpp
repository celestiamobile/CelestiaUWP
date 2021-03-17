#include "pch.h"
#include "CelestiaLocation.h"
#if __has_include("CelestiaLocation.g.cpp")
#include "CelestiaLocation.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaLocation::CelestiaLocation(Location* location) : CelestiaLocationT<CelestiaLocation, CelestiaAstroObject>(location)
	{
	}

	hstring CelestiaLocation::Name()
	{
		return to_hstring(static_cast<Location*>(obj)->getName(true));
	}
}
