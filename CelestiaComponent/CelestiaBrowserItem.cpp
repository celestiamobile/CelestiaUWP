#include "pch.h"
#include "CelestiaBrowserItem.h"
#if __has_include("CelestiaBrowserItem.g.cpp")
#include "CelestiaBrowserItem.g.cpp"
#endif

#include "CelestiaStar.h"
#include "CelestiaBody.h"
#include "CelestiaDSO.h"
#include "CelestiaLocation.h"

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaBrowserItem::CelestiaBrowserItem(hstring name, array_view<CelestiaComponent::CelestiaBrowserItem const> children) : CelestiaBrowserItemT<CelestiaBrowserItem>(), obj(nullptr), provider(nullptr), name(name)
    {
        std::vector<CelestiaComponent::CelestiaBrowserItem> vec;
        for (size_t i = 0; i < children.size(); i++)
        {
            vec.push_back(children[i]);
        }
        this->children = vec;
        areChildrenLoaded = true;
    }

    CelestiaBrowserItem::CelestiaBrowserItem(hstring name, CelestiaComponent::CelestiaAstroObject const& obj, CelestiaComponent::CelestiaBrowserItemChildrenProvider const& provider) : CelestiaBrowserItemT<CelestiaBrowserItem>(), obj(obj), provider(provider), name(name)
    {
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

    com_array<CelestiaComponent::CelestiaBrowserItem> CelestiaBrowserItem::Children()
    {
        if (!areChildrenLoaded && provider != nullptr)
        {
            CelestiaComponent::CelestiaBrowserItem item = *this;
            auto arr = provider(item);
            std::vector<CelestiaComponent::CelestiaBrowserItem> vec;
            for (size_t i = 0; i < arr.size(); i++)
            {
                vec.push_back(arr[i]);
            }
            children = vec;
            areChildrenLoaded = true;
        }
        return com_array<CelestiaComponent::CelestiaBrowserItem>(children);
    }
}
