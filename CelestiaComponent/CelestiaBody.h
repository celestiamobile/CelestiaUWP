//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#pragma once

#include <celengine/body.h>
#include "CelestiaAstroObject.h"
#include "CelestiaOrbit.h"
#include "CelestiaRotationModel.h"

#include "CelestiaBody.g.h"

namespace CelestiaComponent
{
    struct CelestiaPlanetarySystem;
}

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaBody : CelestiaBodyT<CelestiaBody, CelestiaAstroObject>
    {
        CelestiaBody(Body* body);

        hstring Name();
        CelestiaComponent::CelestiaBodyType Type();
        com_array<hstring> AlternateSurfaceNames();
        hstring InfoURL();

        bool HasRings();
        bool HasAtmosphere();
        bool IsEllipsoid();
        float Radius();

        CelestiaComponent::CelestiaPlanetarySystem System();
        CelestiaComponent::CelestiaOrbit OrbitAtTime(Windows::Foundation::DateTime const& time);
        CelestiaComponent::CelestiaRotationModel RotationModelAtTime(Windows::Foundation::DateTime const& time);

        CelestiaComponent::CelestiaPlanetarySystem p {nullptr};
    };
}
