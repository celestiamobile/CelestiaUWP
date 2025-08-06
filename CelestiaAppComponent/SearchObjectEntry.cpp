// SearchObjectEntry.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "SearchObjectEntry.h"
#if __has_include("SearchObjectEntry.g.cpp")
#include "SearchObjectEntry.g.cpp"
#endif

using namespace std;
using namespace winrt;

namespace winrt::CelestiaAppComponent::implementation
{
    SearchObjectEntry::SearchObjectEntry(hstring const& name, hstring const& path) : name(name), path(path)
    {
    }

    hstring SearchObjectEntry::Name()
    {
        return name;
    }

    hstring SearchObjectEntry::Path()
    {
        return path;
    }
}
