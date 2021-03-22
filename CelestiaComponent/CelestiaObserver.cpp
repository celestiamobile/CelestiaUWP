#include "pch.h"
#include "CelestiaObserver.h"
#if __has_include("CelestiaObserver.g.cpp")
#include "CelestiaObserver.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaObserver::CelestiaObserver(Observer* observer) : CelestiaObserverT<CelestiaObserver>(), o(observer)
	{
	}

	void CelestiaObserver::DisplayedSurfaceName(hstring const& displayedSurfaceName)
	{
		o->setDisplayedSurface(to_string(displayedSurfaceName));
	}

	hstring CelestiaObserver::DisplayedSurfaceName()
	{
		return to_hstring(o->getDisplayedSurface());
	}
}
