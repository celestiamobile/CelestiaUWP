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
    Selection::Type GetAstroObjectType(CelestiaComponent::CelestiaAstroObject const& obj)
    {
        if (!obj)
            return Selection::Type_Nil;

        if (obj.try_as<CelestiaComponent::CelestiaBody>() != nullptr)
            return Selection::Type_Body;

        if (obj.try_as<CelestiaComponent::CelestiaStar>() != nullptr)
            return Selection::Type_Star;

        if (obj.try_as<CelestiaComponent::CelestiaDSO>() != nullptr)
            return Selection::Type_DeepSky;

        if (obj.try_as<CelestiaComponent::CelestiaLocation>() != nullptr)
            return Selection::Type_Location;

        return Selection::Type_Nil;
    }

    CelestiaComponent::CelestiaAstroObject GetAstroObject(Selection const& sel)
    {
        switch (sel.getType())
        {
        case Selection::Type_Star:
            return make<CelestiaStar>(sel.star());
        case Selection::Type_DeepSky:
            if (to_hstring(sel.deepsky()->getObjTypeName()) == L"galaxy")
                return make<CelestiaGalaxy>(reinterpret_cast<Galaxy*>(sel.deepsky()));
            return make<CelestiaDSO>(sel.deepsky());
        case Selection::Type_Body:
            return make<CelestiaBody>(sel.body());
        case Selection::Type_Location:
            return make<CelestiaLocation>(sel.location());
        case Selection::Type_Generic:
            return make<CelestiaAstroObject>(sel.object());
        default:
            return nullptr;
        }
    }

    CelestiaSelection::CelestiaSelection() : CelestiaSelectionT<CelestiaSelection>(), object(nullptr), type(Selection::Type_Nil)
    {
    }

    CelestiaSelection::CelestiaSelection(CelestiaComponent::CelestiaAstroObject const& obj) : CelestiaSelectionT<CelestiaSelection>(), object(obj), type(GetAstroObjectType(obj))
    {
    }

    CelestiaSelection::CelestiaSelection(Selection const& sel) : CelestiaSelectionT<CelestiaSelection>(), object(GetAstroObject(sel)), type(Selection::Type_Nil)
    {
        type = GetAstroObjectType(object);
    }

    Selection CelestiaSelection::AsSelection()
    {
        switch (type)
        {
        case Selection::Type_Star:
            return Selection(static_cast<Star*>(get_self<CelestiaAstroObject>(object)->obj));
        case Selection::Type_DeepSky:
            return Selection(static_cast<DeepSkyObject*>(get_self<CelestiaAstroObject>(object)->obj));
        case Selection::Type_Body:
            return Selection(static_cast<Body*>(get_self<CelestiaAstroObject>(object)->obj));
        case Selection::Type_Location:
            return Selection(static_cast<Location*>(get_self<CelestiaAstroObject>(object)->obj));
        case Selection::Type_Generic:
            return Selection(get_self<CelestiaAstroObject>(object)->obj);
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
