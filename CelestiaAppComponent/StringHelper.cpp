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

#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::CelestiaAppComponent::implementation
{
    hstring StringHelper::Join(Collections::IVector<hstring> const& components, hstring const& separator)
    {
        if (components.Size() == 0)
        {
            return L"";
        }

        std::basic_ostringstream<wchar_t> result;

        for (uint32_t i = 0; i < components.Size(); ++i)
        {
            result << std::wstring(components.GetAt(i));

            // Only add the separator if we aren't at the last element
            if (i < components.Size() - 1)
            {
                result << std::wstring(separator);
            }
        }

        return hstring{ result.str() };
    }
}
