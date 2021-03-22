#pragma once

#include <celengine/observer.h>
#include "CelestiaObserver.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaObserver : CelestiaObserverT<CelestiaObserver>
    {
        CelestiaObserver(Observer* observer);

        hstring DisplayedSurfaceName();
        void DisplayedSurfaceName(hstring const& displayedSurfaceName);

        Observer* o;
    };
}