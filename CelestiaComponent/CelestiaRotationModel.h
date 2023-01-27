//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

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

        CelestiaComponent::CelestiaVector AngularVelocityAtTime(Windows::Foundation::DateTime const& time);
        CelestiaComponent::CelestiaVector EquatorOrientationAtTime(Windows::Foundation::DateTime const& time);
        CelestiaComponent::CelestiaVector SpinAtTime(Windows::Foundation::DateTime const& time);

        celestia::ephem::RotationModel* r;
    };
}
