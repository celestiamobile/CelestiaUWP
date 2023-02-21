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
#include "CelestiaLocation.h"

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    Selection::Type GetAstroObjectType(CelestiaComponent::CelestiaAstroObject const& obj)
    {
        if (!obj)
            return Selection::Type_Nil;

        if (obj.try_as<CelestiaBody>() != nullptr)
            return Selection::Type_Body;

        if (obj.try_as<CelestiaStar>() != nullptr)
            return Selection::Type_Star;

        if (obj.try_as<CelestiaDSO>() != nullptr)
            return Selection::Type_DeepSky;

        if (obj.try_as<CelestiaLocation>() != nullptr)
            return Selection::Type_Location;

        return Selection::Type_Nil;
    }

    CelestiaComponent::CelestiaAstroObject GetAstroObject(Selection const& sel)
    {
        switch (sel.getType())
        {
        case Selection::Type_Star:
            return make<CelestiaComponent::implementation::CelestiaStar>(sel.star());
        case Selection::Type_DeepSky:
            return make<CelestiaComponent::implementation::CelestiaDSO>(sel.deepsky());
        case Selection::Type_Body:
            return make<CelestiaComponent::implementation::CelestiaBody>(sel.body());
        case Selection::Type_Location:
            return make<CelestiaComponent::implementation::CelestiaLocation>(sel.location());
        case Selection::Type_Generic:
            return make<CelestiaComponent::implementation::CelestiaAstroObject>(sel.object());
        default:
            return nullptr;
        }
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
}
