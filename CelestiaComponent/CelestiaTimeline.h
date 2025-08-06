// CelestiaTimeline.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <celengine/timeline.h>
#include "CelestiaTimelinePhase.g.h"
#include "CelestiaTimeline.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaTimelinePhase : CelestiaTimelinePhaseT<CelestiaTimelinePhase>
    {
        CelestiaTimelinePhase(TimelinePhase const &tp);

        double StartJulianDay();
        double EndJulianDay();
    private:
        double phaseStartTime;
        double phaseEndTime;
    };

    struct CelestiaTimeline : CelestiaTimelineT<CelestiaTimeline>
    {
        CelestiaTimeline(const Timeline *t);

        int32_t PhaseCount();
        CelestiaComponent::CelestiaTimelinePhase PhaseAtIndex(int32_t index);
    private:
        const Timeline* t;
    };
}
