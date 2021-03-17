#include "pch.h"
#include "CelestiaGalaxy.h"
#if __has_include("CelestiaGalaxy.g.cpp")
#include "CelestiaGalaxy.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaGalaxy::CelestiaGalaxy(Galaxy* galaxy) : CelestiaGalaxyT<CelestiaGalaxy, CelestiaDSO>(galaxy)
	{
	}
}
