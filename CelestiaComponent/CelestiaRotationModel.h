// CelestiaRotationModel.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <celephem/rotation.h>
#include "CelestiaRotationModel.g.h"

namespace CelestiaComponent
{
    struct CelestiaVector;
}

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaRotationModel : CelestiaRotationModelT<CelestiaRotationModel>
    {
        CelestiaRotationModel(celestia::ephem::RotationModel* r);

        bool IsPeriodic();
        double Period();
        double ValidBeginTime();
        double ValidEndTime();

        CelestiaComponent::CelestiaVector AngularVelocityAtTime(double julianDay);
        CelestiaComponent::CelestiaVector EquatorOrientationAtTime(double julianDay);
        CelestiaComponent::CelestiaVector SpinAtTime(double julianDay);

        celestia::ephem::RotationModel* r;
    };
}
