#pragma once

#include <celengine/body.h>
#include "CelestiaAstroObject.h"
#include "CelestiaBody.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaBody : CelestiaBodyT<CelestiaBody, CelestiaAstroObject>
    {
        CelestiaBody(Body* body);

        hstring Name();
    };
}