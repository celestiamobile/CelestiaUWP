#pragma once

#include <celengine/astroobj.h>
#include "CelestiaAstroObject.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaAstroObject : CelestiaAstroObjectT<CelestiaAstroObject>
    {
        CelestiaAstroObject(AstroObject* obj);
    protected:
        AstroObject* obj;
    };
}