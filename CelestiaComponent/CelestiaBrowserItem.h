#pragma once

#include "CelestiaAstroObject.h"
#include "CelestiaBrowserItem.g.h"

namespace winrt::CelestiaComponent
{
    struct CelestiaBrowserItem;
}

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaBrowserItem : CelestiaBrowserItemT<CelestiaBrowserItem>
    {
        CelestiaBrowserItem(hstring name, array_view<CelestiaComponent::CelestiaBrowserItem const> children);
        CelestiaBrowserItem(hstring name, CelestiaComponent::CelestiaAstroObject const& obj, CelestiaComponent::CelestiaBrowserItemChildrenProvider const& provider);

        CelestiaComponent::CelestiaBrowserItemChildrenProvider Provider();

        hstring Name();
        CelestiaComponent::CelestiaAstroObject Object();

        com_array<CelestiaComponent::CelestiaBrowserItem> Children();

        CelestiaComponent::CelestiaAstroObject obj;
        CelestiaComponent::CelestiaBrowserItemChildrenProvider provider;
        hstring name;
        std::vector<CelestiaComponent::CelestiaBrowserItem> children;
        bool areChildrenLoaded;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaBrowserItem : CelestiaBrowserItemT<CelestiaBrowserItem, implementation::CelestiaBrowserItem>
    {
    };
}
