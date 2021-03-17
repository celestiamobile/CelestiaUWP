#pragma once

#include <celengine/simulation.h>
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

        Simulation* sim;
        CelestiaComponent::CelestiaUniverse u;
    };
}