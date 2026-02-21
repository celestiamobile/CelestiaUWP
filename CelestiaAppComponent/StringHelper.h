// StringHelper.h
//
// Copyright (C) 2026, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "StringHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct StringHelper : StringHelperT<StringHelper>
    {
        static hstring Join(Windows::Foundation::Collections::IVector<hstring> const& components, hstring const& separator);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct StringHelper : StringHelperT<StringHelper, implementation::StringHelper>
    {
    };
}
