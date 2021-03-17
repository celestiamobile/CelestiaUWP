#include "pch.h"
#include "CelestiaAstroObject.h"
#if __has_include("CelestiaAstroObject.g.cpp")
#include "CelestiaAstroObject.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaAstroObject::CelestiaAstroObject(AstroObject* obj) : CelestiaAstroObjectT<CelestiaAstroObject>(), obj(obj)
	{
	}
}
