#include "pch.h"
#include "CelestiaGotoLocation.h"
#if __has_include("CelestiaGotoLocation.g.cpp")
#include "CelestiaGotoLocation.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaGotoLocation::CelestiaGotoLocation(CelestiaComponent::CelestiaSelection const& selection, Windows::Foundation::IReference<double> latitude, Windows::Foundation::IReference<double> longitude, Windows::Foundation::IReference<double> distance, Windows::Foundation::IReference<CelestiaComponent::CelestiaGotoLocationDistanceUnit> unit) :
		CelestiaGotoLocationT<CelestiaGotoLocation>(),
		selection(selection),
		latitude(latitude),
		longitude(longitude),
		distance(distance),
		unit(unit)
	{
	}
}
