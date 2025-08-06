// CelestiaBrowserItem.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaBrowserItem.h"
#if __has_include("CelestiaBrowserItem.g.cpp")
#include "CelestiaBrowserItem.g.cpp"
#endif

#include "CelestiaStar.h"
#include "CelestiaBody.h"
#include "CelestiaDSO.h"
#include "CelestiaLocation.h"

#include <stringapiset.h>

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::CelestiaComponent::implementation
{
    bool CompareBrowserItems(const CelestiaComponent::CelestiaBrowserItem& lhs, const CelestiaComponent::CelestiaBrowserItem& rhs)
    {
        return CompareStringEx(nullptr, SORT_DIGITSASNUMBERS, lhs.Name().c_str(), lhs.Name().size(), rhs.Name().c_str(), rhs.Name().size(), nullptr, nullptr, 0) == CSTR_LESS_THAN;
    }

    CelestiaBrowserItem::CelestiaBrowserItem(hstring name, array_view<CelestiaComponent::CelestiaBrowserItem const> children, bool sorted) : CelestiaBrowserItemT<CelestiaBrowserItem>(), obj(nullptr), provider(nullptr), name(name), sorted(sorted)
    {
        std::vector<CelestiaComponent::CelestiaBrowserItem> vec;
        for (const auto& child : children)
        {
            vec.push_back(child);
        }
        if (!sorted)
            std::sort(vec.begin(), vec.end(), CompareBrowserItems);
        this->children = single_threaded_observable_vector<CelestiaComponent::CelestiaBrowserItem>({ vec.begin(), vec.end()});
        areChildrenLoaded = true;
    }

    CelestiaBrowserItem::CelestiaBrowserItem(hstring name, CelestiaComponent::CelestiaAstroObject const& obj, CelestiaComponent::CelestiaBrowserItemChildrenProvider const& provider, bool sorted) : CelestiaBrowserItemT<CelestiaBrowserItem>(), obj(obj), provider(provider), name(name), sorted(sorted)
    {
        children = single_threaded_observable_vector<CelestiaComponent::CelestiaBrowserItem>();
        areChildrenLoaded = false;
    }

    CelestiaComponent::CelestiaBrowserItemChildrenProvider CelestiaBrowserItem::Provider()
    {
        return provider;
    }

    hstring CelestiaBrowserItem::Name()
    {
        return name;
    }

    CelestiaComponent::CelestiaAstroObject CelestiaBrowserItem::Object()
    {
        return obj;
    }

    Collections::IObservableVector<CelestiaComponent::CelestiaBrowserItem> CelestiaBrowserItem::Children()
    {
        if (!areChildrenLoaded && provider != nullptr)
        {
            CelestiaComponent::CelestiaBrowserItem item = *this;
            auto arr = provider(item);
            std::vector<CelestiaComponent::CelestiaBrowserItem> vec;
            for (const auto& child : arr)
            {
                vec.push_back(child);
            }
            if (!sorted)
                std::sort(vec.begin(), vec.end(), CompareBrowserItems);
            children.ReplaceAll(vec);
            areChildrenLoaded = true;
        }
        return children;
    }
}
