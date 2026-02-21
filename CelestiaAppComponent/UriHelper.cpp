// UriHelper.cpp
//
// Copyright (C) 2026, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "UriHelper.h"
#if __has_include("UriHelper.g.cpp")
#include "UriHelper.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::CelestiaAppComponent::implementation
{
    Collections::IVector<hstring> UriHelper::PathSegments(Uri const& uri)
    {
        auto segments = single_threaded_vector<hstring>();

        std::wstring_view path = uri.Path();

        // 1. Remove leading slash if present
        if (!path.empty() && path[0] == L'/')
        {
            path.remove_prefix(1);
        }

        // 2. Use a wistringstream or manual search to split by '/'
        size_t start = 0;
        size_t end = path.find(L'/');

        while (start < path.length())
        {
            // Handle the last segment or a single segment
            if (end == std::wstring_view::npos)
            {
                end = path.length();
            }

            std::wstring_view segment = path.substr(start, end - start);

            // Only add non-empty segments (ignores double slashes //)
            if (!segment.empty())
            {
                // 3. Unescape each part individually
                segments.Append(Uri::UnescapeComponent(hstring(segment)));
            }

            if (end == path.length()) break;

            start = end + 1;
            end = path.find(L'/', start);
        }

        return segments;
    }
}
