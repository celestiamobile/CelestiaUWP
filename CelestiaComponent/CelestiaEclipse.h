#pragma once

#include <celestia/eclipsefinder.h>
#include "CelestiaBody.h"
#include "CelestiaEclipse.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaEclipse : CelestiaEclipseT<CelestiaEclipse>
    {
        CelestiaEclipse(Eclipse* e);

        CelestiaComponent::CelestiaBody Occulter();
        CelestiaComponent::CelestiaBody Receiver();
        double StartTime();
        double EndTime();

        CelestiaComponent::CelestiaBody occulter;
        CelestiaComponent::CelestiaBody receiver;

        double startTime;
        double endTime;
    };
}