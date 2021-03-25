#pragma once

#include <celengine/deepskyobj.h>
#include "CelestiaAstroObject.h"
#include "CelestiaDSO.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaDSO : CelestiaDSOT<CelestiaDSO, CelestiaAstroObject>
    {
        CelestiaDSO(DeepSkyObject* dso);
        hstring Type();
        hstring InfoURL();
    };
}