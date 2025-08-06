//
// SelectionHelper.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "SelectionHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct SelectionHelper : SelectionHelperT<SelectionHelper>
    {
        static hstring GetOverview(CelestiaComponent::CelestiaSelection const& selection, CelestiaComponent::CelestiaAppCore const& appCore);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct SelectionHelper : SelectionHelperT<SelectionHelper, implementation::SelectionHelper>
    {
    };
}
