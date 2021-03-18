#include "pch.h"
#include "CelestiaDestination.h"
#if __has_include("CelestiaDestination.g.cpp")
#include "CelestiaDestination.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaDestination::CelestiaDestination(Destination const* d) : CelestiaDestinationT<CelestiaDestination>(), name(to_hstring(d->name)), target(to_hstring(d->target)), content(to_hstring(d->description)), distance(d->distance)
	{
	}

	hstring CelestiaDestination::Name()
	{
		return name;
	}

	hstring CelestiaDestination::Target()
	{
		return target;
	}

	hstring CelestiaDestination::Content()
	{
		return content;
	}

	double CelestiaDestination::Distance()
	{
		return distance;
	}
}
