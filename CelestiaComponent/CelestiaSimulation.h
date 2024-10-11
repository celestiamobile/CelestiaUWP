//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include <celengine/simulation.h>
#include "CelestiaCompletion.h"
#include "CelestiaDestination.h"
#include "CelestiaEclipse.h"
#include "CelestiaGotoLocation.h"
#include "CelestiaObserver.h"
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

        Windows::Foundation::DateTime Time();
        void Time(Windows::Foundation::DateTime const& time);
        double JulianDay();
        void JulianDay(double jd);

        CelestiaComponent::CelestiaUniverse Universe();
        CelestiaComponent::CelestiaObserver ActiveObserver();

        CelestiaComponent::CelestiaSelection Find(hstring const& name);

        void GoToDestination(CelestiaComponent::CelestiaDestination const& destination);
        void GoToLocation(CelestiaComponent::CelestiaGotoLocation const& location);
        void GoToEclipse(CelestiaComponent::CelestiaEclipse const& eclipse);
        void ReverseOrientation();

        com_array<CelestiaComponent::CelestiaCompletion> GetCompletion(hstring const& name);

        Simulation* sim;
        CelestiaComponent::CelestiaUniverse u;
    };
}
