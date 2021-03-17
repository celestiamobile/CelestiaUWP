#include "pch.h"
#include "CelestiaBody.h"
#if __has_include("CelestiaBody.g.cpp")
#include "CelestiaBody.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaBody::CelestiaBody(Body* body) : CelestiaBodyT<CelestiaBody, CelestiaAstroObject>(body)
	{
	}

	hstring CelestiaBody::Name()
	{
		return to_hstring(static_cast<Body*>(obj)->getName(true));
	}
}
