// CelestiaTimeline.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaTimeline.h"
#include "CelestiaHelper.h"
#if __has_include("CelestiaTimelinePhase.g.cpp")
#include "CelestiaTimelinePhase.g.cpp"
#endif
#if __has_include("CelestiaTimeline.g.cpp")
#include "CelestiaTimeline.g.cpp"
#endif

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaTimelinePhase::CelestiaTimelinePhase(TimelinePhase const& tp) : phaseStartTime(tp.startTime()), phaseEndTime(tp.endTime())
    {
    }

    double CelestiaTimelinePhase::StartJulianDay()
    {
        return phaseStartTime;
    }

    double CelestiaTimelinePhase::EndJulianDay()
    {
        return phaseEndTime;
    }

    CelestiaTimeline::CelestiaTimeline(const Timeline* t) : t(t)
    {
    }

    int32_t CelestiaTimeline::PhaseCount()
    {
        return t->phaseCount();
    }

    CelestiaComponent::CelestiaTimelinePhase CelestiaTimeline::PhaseAtIndex(int32_t index)
    {
        return make<CelestiaTimelinePhase>(t->getPhase(static_cast<unsigned int>(index)));
    }
}
