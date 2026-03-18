// FeatureFlags.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "FeatureFlags.g.h"
#include <unordered_map>

namespace winrt::CelestiaWinUI::implementation
{
    struct FeatureFlags : FeatureFlagsT<FeatureFlags>
    {
        FeatureFlags();
        FeatureFlags(std::unordered_map<hstring, bool> const& flags);

        bool Dummy();

        static Windows::Foundation::IAsyncAction UpdateAsync(bool isXbox, Windows::Storage::ApplicationDataContainer const& settings);
        static CelestiaWinUI::FeatureFlags Get(Windows::Storage::ApplicationDataContainer const& settings);

    private:
        std::unordered_map<hstring, bool> flags;

        bool Lookup(hstring const& key);
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct FeatureFlags : FeatureFlagsT<FeatureFlags, implementation::FeatureFlags>
    {
    };
}
