#pragma once

#include <celengine/selection.h>
#include "CelestiaAstroObject.h"
#include "CelestiaSelection.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaSelection : CelestiaSelectionT<CelestiaSelection>
    {
        CelestiaSelection(CelestiaComponent::CelestiaAstroObject const& obj);
        CelestiaSelection(Selection const& sel);

        CelestiaComponent::CelestiaAstroObject Object();
        bool IsEmpty();

        ~CelestiaSelection();

        Selection* s;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaSelection : CelestiaSelectionT<CelestiaSelection, implementation::CelestiaSelection>
    {
    };
}
