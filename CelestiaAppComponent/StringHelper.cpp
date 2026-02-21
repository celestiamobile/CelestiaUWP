// StringHelper.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "StringHelper.h"
#if __has_include("StringHelper.g.cpp")
#include "StringHelper.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::CelestiaAppComponent::implementation
{
    hstring StringHelper::Join(Collections::IVector<hstring const> const components, hstring const& separator)
    {
        return L"";
    }
}
