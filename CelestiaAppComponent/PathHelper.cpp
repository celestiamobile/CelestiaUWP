// PathHelper.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "PathHelper.h"
#if __has_include("PathHelper.g.cpp")
#include "PathHelper.g.cpp"
#endif

#include <PathCch.h>

namespace winrt::CelestiaAppComponent::implementation
{
    hstring PathHelper::Combine(hstring const& path1, hstring const& path2)
    {
        WCHAR* result;
        HRESULT hr = PathAllocCombine(path1.c_str(), path2.c_str(), PATHCCH_ALLOW_LONG_PATHS, &result);
        if (FAILED(hr))
            return path1 + L"\\" + path2;
        hstring resultString = result;
        LocalFree(result);
        return resultString;
    }
}
