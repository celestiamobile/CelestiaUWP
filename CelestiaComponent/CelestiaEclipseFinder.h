#pragma once

#include "CelestiaBody.h"
#include "CelestiaEclipse.h"
#include "CelestiaEclipseFinder.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaEclipseFinder : CelestiaEclipseFinderT<CelestiaEclipseFinder>
    {
        CelestiaEclipseFinder(CelestiaComponent::CelestiaBody body);
        com_array<CelestiaComponent::CelestiaEclipse> Search(CelestiaComponent::CelestiaEclipseKind kind, double startTime, double endTime);
        ~CelestiaEclipseFinder();

        EclipseFinder* f;
        EclipseFinderWatcher* w;
        bool aborted;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaEclipseFinder : CelestiaEclipseFinderT<CelestiaEclipseFinder, implementation::CelestiaEclipseFinder>
    {
    };
}
