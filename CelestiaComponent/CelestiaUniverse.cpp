#include "pch.h"
#include "CelestiaUniverse.h"
#if __has_include("CelestiaUniverse.g.cpp")
#include "CelestiaUniverse.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaUniverse::CelestiaUniverse(Universe* u) : CelestiaUniverseT<CelestiaUniverse>(), u(u)
	{
	}

	CelestiaComponent::CelestiaSelection CelestiaUniverse::Find(hstring const& name)
	{
		return make<CelestiaSelection>(u->find(to_string(name)));
	}
}
