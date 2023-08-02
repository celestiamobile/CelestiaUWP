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
