// CelestiaOrbit.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaOrbit.h"
#include "CelestiaVector.h"
#if __has_include("CelestiaOrbit.g.cpp")
#include "CelestiaOrbit.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaOrbit::CelestiaOrbit(celestia::ephem::Orbit* o) : CelestiaOrbitT<CelestiaOrbit>(), o(o)
    {
    }

    bool CelestiaOrbit::IsPeriodic()
    {
        return o->isPeriodic();
    }

    double CelestiaOrbit::Period()
    {
        return o->getPeriod();
    }

    double CelestiaOrbit::ValidBeginTime()
    {
        double begin, end;
        o->getValidRange(begin, end);
        return begin;
    }

    double CelestiaOrbit::ValidEndTime()
    {
        double begin, end;
        o->getValidRange(begin, end);
        return end;
    }

    double CelestiaOrbit::BoundingRadius()
    {
        return o->getBoundingRadius();
    }

    CelestiaComponent::CelestiaVector CelestiaOrbit::VelocityAtTime(double julianDay)
    {
        const Eigen::Vector3d v = o->velocityAtTime(julianDay);
        return make<CelestiaVector>(v.x(), v.y(), v.z());
    }

    CelestiaComponent::CelestiaVector CelestiaOrbit::PositionAtTime(double julianDay)
    {
        const Eigen::Vector3d v = o->positionAtTime(julianDay);
        return make<CelestiaVector>(v.x(), v.y(), v.z());
    }
}
