// SearchObjectEntry.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "SearchObjectEntry.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct SearchObjectEntry : SearchObjectEntryT<SearchObjectEntry>
    {
        SearchObjectEntry(hstring const& name, hstring const& path);
        hstring Name();
        hstring Path();

    private:
        hstring name;
        hstring path;
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct SearchObjectEntry : SearchObjectEntryT<SearchObjectEntry, implementation::SearchObjectEntry>
    {
    };
}
