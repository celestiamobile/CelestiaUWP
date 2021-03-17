#pragma once

#include <celengine/astroobj.h>
#include "CelestiaAstroObject.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaAstroObject : CelestiaAstroObjectT<CelestiaAstroObject>
    {
    public:
        CelestiaAstroObject(AstroObject* obj);
        AstroObject* obj;
    };
}