// SystemInformation.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "SystemInformation.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct SystemInformation : SystemInformationT<SystemInformation>
    {
        SystemInformation();
        Windows::ApplicationModel::PackageVersion ApplicationVersion();
        hstring OperatingSystem();
        hstring OperatingSystemVersion();
        Windows::System::ProcessorArchitecture OperatingSystemArchitecture();
        hstring OperatingSystemArchitectureString();
        hstring DeviceModel();
        hstring DeviceManufacturer();
        hstring DeviceFamily();

        static CelestiaAppComponent::SystemInformation Instance();

    private:
        Windows::ApplicationModel::PackageVersion applicationVersion;
        hstring operatingSystem;
        hstring operatingSystemVersion;
        Windows::System::ProcessorArchitecture operatingSystemArchitecture;
        hstring deviceModel;
        hstring deviceManufacturer;
        hstring deviceFamily;
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct SystemInformation : SystemInformationT<SystemInformation, implementation::SystemInformation>
    {
    };
}
