//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

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
        CelestiaBrowserItem(hstring name, array_view<CelestiaComponent::CelestiaBrowserItem const> children, bool sorted);
        CelestiaBrowserItem(hstring name, CelestiaComponent::CelestiaAstroObject const& obj, CelestiaComponent::CelestiaBrowserItemChildrenProvider const& provider, bool sorted);

        CelestiaComponent::CelestiaBrowserItemChildrenProvider Provider();

        hstring Name();
        CelestiaComponent::CelestiaAstroObject Object();

        Windows::Foundation::Collections::IObservableVector<CelestiaComponent::CelestiaBrowserItem> Children();

        CelestiaComponent::CelestiaAstroObject obj;
        CelestiaComponent::CelestiaBrowserItemChildrenProvider provider;
        hstring name;
        Windows::Foundation::Collections::IObservableVector<CelestiaComponent::CelestiaBrowserItem> children;
        bool areChildrenLoaded;
        bool sorted;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaBrowserItem : CelestiaBrowserItemT<CelestiaBrowserItem, implementation::CelestiaBrowserItem>
    {
    };
}
