#pragma once

#include <celengine/simulation.h>
#include "CelestiaDestination.h"
#include "CelestiaEclipse.h"
#include "CelestiaGotoLocation.h"
#include "CelestiaObserver.h"
#include "CelestiaSelection.h"
#include "CelestiaStarBrowser.h"
#include "CelestiaUniverse.h"
#include "CelestiaSimulation.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaSimulation : CelestiaSimulationT<CelestiaSimulation>
    {
        CelestiaSimulation(Simulation* sim);

        CelestiaComponent::CelestiaSelection Selection();
        void Selection(CelestiaComponent::CelestiaSelection const& selection);

        Windows::Foundation::DateTime Time();
        void Time(Windows::Foundation::DateTime const& time);

        CelestiaComponent::CelestiaUniverse Universe();
        CelestiaComponent::CelestiaObserver ActiveObserver();

        CelestiaComponent::CelestiaSelection Find(hstring const& name);

        CelestiaComponent::CelestiaStarBrowser StarBrowser(CelestiaComponent::CelestiaStarBrowserType type);

        void GoToDestination(CelestiaComponent::CelestiaDestination const& destination);
        void GoToLocation(CelestiaComponent::CelestiaGotoLocation const& location);
        void GoToEclipse(CelestiaComponent::CelestiaEclipse const& eclipse);

        Simulation* sim;
        CelestiaComponent::CelestiaUniverse u;
    };
}