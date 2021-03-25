#pragma once

#include <celengine/star.h>
#include "CelestiaAstroObject.h"
#include "CelestiaStar.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaStar : CelestiaStarT<CelestiaStar, CelestiaAstroObject>
    {
        CelestiaStar(Star* star);
        hstring InfoURL();
    };
}