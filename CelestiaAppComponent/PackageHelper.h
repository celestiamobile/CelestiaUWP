// PackageHelper.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "PackageHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct PackageHelper : PackageHelperT<PackageHelper>
    {
        static hstring InstalledLocationPath();
        static hstring ExecutablePath();
        static Windows::ApplicationModel::PackageVersion ApplicationVersion();
        static Windows::System::ProcessorArchitecture ProcessorArchitecture();
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct PackageHelper : PackageHelperT<PackageHelper, implementation::PackageHelper>
    {
    };
}
