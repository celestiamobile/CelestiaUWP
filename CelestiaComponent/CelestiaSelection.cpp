//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaSelection.h"
#if __has_include("CelestiaSelection.g.cpp")
#include "CelestiaSelection.g.cpp"
#endif

#include "CelestiaStar.h"
#include "CelestiaBody.h"
#include "CelestiaDSO.h"
#include "CelestiaGalaxy.h"
#include "CelestiaLocation.h"

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    SelectionType GetAstroObjectType(CelestiaComponent::CelestiaAstroObject const& obj)
    {
        if (!obj)
            return SelectionType::None;

        if (obj.try_as<CelestiaComponent::CelestiaBody>() != nullptr)
            return SelectionType::Body;

        if (obj.try_as<CelestiaComponent::CelestiaStar>() != nullptr)
            return SelectionType::Star;

        if (obj.try_as<CelestiaComponent::CelestiaDSO>() != nullptr)
            return SelectionType::DeepSky;

        if (obj.try_as<CelestiaComponent::CelestiaLocation>() != nullptr)
            return SelectionType::Location;

        return SelectionType::None;
    }

    CelestiaComponent::CelestiaAstroObject GetAstroObject(Selection const& sel)
    {
        switch (sel.getType())
        {
        case SelectionType::Star:
            return make<CelestiaStar>(sel.star());
        case SelectionType::DeepSky:
            if (to_hstring(sel.deepsky()->getObjTypeName()) == L"galaxy")
                return make<CelestiaGalaxy>(reinterpret_cast<Galaxy*>(sel.deepsky()));
            return make<CelestiaDSO>(sel.deepsky());
        case SelectionType::Body:
            return make<CelestiaBody>(sel.body());
        case SelectionType::Location:
            return make<CelestiaLocation>(sel.location());
        default:
            return nullptr;
        }
    }

    CelestiaSelection::CelestiaSelection() : CelestiaSelectionT<CelestiaSelection>(), object(nullptr), type(SelectionType::None)
    {
    }

    CelestiaSelection::CelestiaSelection(CelestiaComponent::CelestiaAstroObject const& obj) : CelestiaSelectionT<CelestiaSelection>(), object(obj), type(GetAstroObjectType(obj))
    {
    }

    CelestiaSelection::CelestiaSelection(Selection const& sel) : CelestiaSelectionT<CelestiaSelection>(), object(GetAstroObject(sel)), type(SelectionType::None)
    {
        type = GetAstroObjectType(object);
    }

    Selection CelestiaSelection::AsSelection()
    {
        switch (type)
        {
        case SelectionType::Star:
            return Selection(static_cast<Star*>(get_self<CelestiaAstroObject>(object)->obj));
        case SelectionType::DeepSky:
            return Selection(static_cast<DeepSkyObject*>(get_self<CelestiaAstroObject>(object)->obj));
        case SelectionType::Body:
            return Selection(static_cast<Body*>(get_self<CelestiaAstroObject>(object)->obj));
        case SelectionType::Location:
            return Selection(static_cast<Location*>(get_self<CelestiaAstroObject>(object)->obj));
        default:
            return Selection();
        }
    }

    CelestiaComponent::CelestiaAstroObject CelestiaSelection::Object()
    {
        return object;
    }

    bool CelestiaSelection::IsEmpty()
    {
        return AsSelection().empty();
    }

    double CelestiaSelection::Radius()
    {
        return AsSelection().radius();
    }

    hstring CelestiaSelection::InfoURL()
    {
        if (object == nullptr) return L"";
        auto body = object.try_as<CelestiaComponent::CelestiaBody>();
        if (body != nullptr)
        {
            return body.InfoURL();
        }
        auto star = object.try_as<CelestiaComponent::CelestiaStar>();
        if (star != nullptr)
        {
            return star.InfoURL();
        }
        auto dso = object.try_as<CelestiaComponent::CelestiaDSO>();
        if (dso != nullptr)
        {
            return dso.InfoURL();
        }
        return L"";
    }
}
