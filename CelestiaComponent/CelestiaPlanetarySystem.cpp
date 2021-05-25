//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaBody.h"
#include "CelestiaHelper.h"
#include "CelestiaStar.h"
#include "CelestiaPlanetarySystem.h"
#if __has_include("CelestiaPlanetarySystem.g.cpp")
#include "CelestiaPlanetarySystem.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaPlanetarySystem::CelestiaPlanetarySystem(PlanetarySystem* p) : CelestiaPlanetarySystemT<CelestiaPlanetarySystem>(), p(p)
    {
    }

    CelestiaComponent::CelestiaBody CelestiaPlanetarySystem::PrimaryObject()
    {
        return make<CelestiaBody>(p->getPrimaryBody());
    }

    CelestiaComponent::CelestiaStar CelestiaPlanetarySystem::Star()
    {
        return make<CelestiaStar>(p->getStar());
    }
}
