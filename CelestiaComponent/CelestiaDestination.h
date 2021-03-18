#pragma once

#include <celestia/destination.h>
#include "CelestiaDestination.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaDestination : CelestiaDestinationT<CelestiaDestination>
    {
        CelestiaDestination(Destination const* d);

        hstring Name();
        hstring Target();
        hstring Content();
        double Distance();

        hstring name;
        hstring target;
        hstring content;
        double distance;
    };
}