#pragma once

#include <celengine/starbrowser.h>
#include "CelestiaStar.h"
#include "CelestiaStarBrowser.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaStarBrowser : CelestiaStarBrowserT<CelestiaStarBrowser>
    {
        CelestiaStarBrowser(StarBrowser* sb);
        com_array<CelestiaComponent::CelestiaStar> Stars();

        ~CelestiaStarBrowser();

    private:
        StarBrowser* sb;
    };
}