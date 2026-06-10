// PackageHelper.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "PackageHelper.h"
#if __has_include("PackageHelper.g.cpp")
#include "PackageHelper.g.cpp"
#endif

#include "AppDataHelper.h"

#include <filesystem>
#include <vector>
#include <winver.h>

#pragma comment(lib, "version.lib")

namespace winrt::CelestiaAppComponent::implementation
{
    static std::wstring GetExecutablePath()
    {
        std::wstring buf(MAX_PATH, L'\0');
        DWORD length = ::GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
        while (length == buf.size())
        {
            buf.resize(buf.size() * 2);
            length = ::GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
        }
        buf.resize(length);
        return buf;
    }

    hstring PackageHelper::InstalledLocationPath()
    {
        if (AppDataHelper::IsPackaged())
            return Windows::ApplicationModel::Package::Current().InstalledLocation().Path();

        std::filesystem::path exePath{ GetExecutablePath() };
        return hstring{ exePath.parent_path().c_str() };
    }

    Windows::ApplicationModel::PackageVersion PackageHelper::ApplicationVersion()
    {
        if (AppDataHelper::IsPackaged())
            return Windows::ApplicationModel::Package::Current().Id().Version();

        Windows::ApplicationModel::PackageVersion version{};
        auto exePath = GetExecutablePath();

        DWORD handle = 0;
        DWORD size = ::GetFileVersionInfoSizeW(exePath.c_str(), &handle);
        if (size == 0) return version;
        std::vector<BYTE> data(size);
        if (!::GetFileVersionInfoW(exePath.c_str(), 0, size, data.data())) return version;
        VS_FIXEDFILEINFO* info = nullptr;
        UINT infoSize = 0;
        if (!::VerQueryValueW(data.data(), L"\\", reinterpret_cast<LPVOID*>(&info), &infoSize) || info == nullptr)
            return version;
        version.Major = static_cast<uint16_t>(HIWORD(info->dwFileVersionMS));
        version.Minor = static_cast<uint16_t>(LOWORD(info->dwFileVersionMS));
        version.Build = static_cast<uint16_t>(HIWORD(info->dwFileVersionLS));
        version.Revision = static_cast<uint16_t>(LOWORD(info->dwFileVersionLS));
        return version;
    }

    Windows::System::ProcessorArchitecture PackageHelper::ProcessorArchitecture()
    {
        if (AppDataHelper::IsPackaged())
            return Windows::ApplicationModel::Package::Current().Id().Architecture();
#if defined(_M_ARM64)
        return Windows::System::ProcessorArchitecture::Arm64;
#elif defined(_M_X64)
        return Windows::System::ProcessorArchitecture::X64;
#elif defined(_M_IX86)
        return Windows::System::ProcessorArchitecture::X86;
#elif defined(_M_ARM)
        return Windows::System::ProcessorArchitecture::Arm;
#else
        return Windows::System::ProcessorArchitecture::Unknown;
#endif
    }
}
