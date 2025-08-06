// PathHelper.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "PathHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct PathHelper : PathHelperT<PathHelper>
    {
        static hstring Combine(hstring const& path1, hstring const& path2);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct PathHelper : PathHelperT<PathHelper, implementation::PathHelper>
    {
    };
}
