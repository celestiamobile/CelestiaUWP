#include "pch.h"
#include "CelestiaDSO.h"
#if __has_include("CelestiaDSO.g.cpp")
#include "CelestiaDSO.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaDSO::CelestiaDSO(DeepSkyObject* dso) : CelestiaDSOT<CelestiaDSO, CelestiaAstroObject>(dso)
	{
	}
}
