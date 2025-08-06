// SystemInformation.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "SystemInformation.h"
#if __has_include("SystemInformation.g.cpp")
#include "SystemInformation.g.cpp"
#endif

#include <fmt/format.h>
#include <fmt/xchar.h>

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Data::Json;

namespace winrt::CelestiaAppComponent::implementation
{
    SystemInformation::SystemInformation()
    {
        auto packageId = Windows::ApplicationModel::Package::Current().Id();
        applicationVersion = packageId.Version();
        operatingSystemArchitecture = packageId.Architecture();
        Windows::Security::ExchangeActiveSyncProvisioning::EasClientDeviceInformation deviceInfo;
        operatingSystem = deviceInfo.OperatingSystem();
        deviceModel = deviceInfo.SystemProductName();
        deviceManufacturer = deviceInfo.SystemManufacturer();
        auto versionInfo = Windows::System::Profile::AnalyticsInfo::VersionInfo();
        deviceFamily = versionInfo.DeviceFamily();
        auto versionString = to_string(versionInfo.DeviceFamilyVersion());
        unsigned long long version = 0;
        auto [ptr, ec] = std::from_chars(versionString.data(), versionString.data() + versionString.size(), version);
        if (ec != std::errc{} || ptr != versionString.data() + versionString.size())
            version = 0;
        operatingSystemVersion = hstring(fmt::format(L"{0}.{1}.{2}.{3}", (version & 0xFFFF000000000000L) >> 48, (version & 0x0000FFFF00000000L) >> 32, (version & 0x00000000FFFF0000L) >> 16, (version & 0x000000000000FFFFL)));
    }

    Windows::ApplicationModel::PackageVersion SystemInformation::ApplicationVersion()
    {
        return applicationVersion;
    }

    hstring SystemInformation::OperatingSystem()
    {
        return operatingSystem;
    }

    hstring SystemInformation::OperatingSystemVersion()
    {
        return operatingSystemVersion;
    }

    Windows::System::ProcessorArchitecture SystemInformation::OperatingSystemArchitecture()
    {
        return operatingSystemArchitecture;
    }

    hstring SystemInformation::OperatingSystemArchitectureString()
    {
        switch (operatingSystemArchitecture)
        {
        case Windows::System::ProcessorArchitecture::X86:
            return L"X86";
        case Windows::System::ProcessorArchitecture::Arm:
            return L"ARM";
        case Windows::System::ProcessorArchitecture::X64:
            return L"X64";
        case Windows::System::ProcessorArchitecture::Neutral:
            return L"Neutral";
        case Windows::System::ProcessorArchitecture::Arm64:
            return L"ARM64";
        case Windows::System::ProcessorArchitecture::X86OnArm64:
            return L"X86OnARM64";
        case Windows::System::ProcessorArchitecture::Unknown:
        default:
            return L"Unknown";
        }
    }

    hstring SystemInformation::DeviceModel()
    {
        return deviceModel;
    }

    hstring SystemInformation::DeviceManufacturer()
    {
        return deviceManufacturer;
    }

    hstring SystemInformation::DeviceFamily()
    {
        return deviceFamily;
    }

    CelestiaAppComponent::SystemInformation SystemInformation::Instance()
    {
        static CelestiaAppComponent::SystemInformation instance = nullptr;
        if (instance == nullptr)
            instance = make<SystemInformation>();
        return instance;
    }
}
