#pragma once

#include <celengine/simulation.h>
#include "CelestiaDestination.h"
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
        void GoToDestination(CelestiaComponent::CelestiaDestination destination);

        Simulation* sim;
        CelestiaComponent::CelestiaUniverse u;
    };
}