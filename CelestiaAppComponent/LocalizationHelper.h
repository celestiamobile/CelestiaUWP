// LocalizationHelper.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "LocalizationHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct LocalizationHelper : LocalizationHelperT<LocalizationHelper>
    {
        static hstring Locale();
        static void Locale(hstring const& locale);
        static hstring Localize(hstring const& original, hstring const& comment);
        static hstring Localize(hstring const& original, hstring const& context, hstring const& comment);
        static hstring FromWindowsTag(hstring const& tag);
        static hstring ToWindowsTag(hstring const& locale);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct LocalizationHelper : LocalizationHelperT<LocalizationHelper, implementation::LocalizationHelper>
    {
    };
}
