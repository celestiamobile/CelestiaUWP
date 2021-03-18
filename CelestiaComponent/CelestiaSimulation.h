#pragma once

#include <celengine/simulation.h>
#include "CelestiaDestination.h"
#include "CelestiaGotoLocation.h"
#include "CelestiaSelection.h"
#include "CelestiaUniverse.h"
#include "CelestiaSimulation.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaSimulation : CelestiaSimulationT<CelestiaSimulation>
    {
        CelestiaSimulation(Simulation* sim);

        CelestiaComponent::CelestiaSelection Selection();
        void Selection(CelestiaComponent::CelestiaSelection const& selection);

        CelestiaComponent::CelestiaUniverse Universe();

        CelestiaComponent::CelestiaSelection Find(hstring const& name);
        void GoToDestination(CelestiaComponent::CelestiaDestination const& destination);
        void GoToLocation(CelestiaComponent::CelestiaGotoLocation const& location);

        Simulation* sim;
        CelestiaComponent::CelestiaUniverse u;
    };
}