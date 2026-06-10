// AppDataHelper.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "AppDataHelper.h"
#if __has_include("AppDataHelper.g.cpp")
#include "AppDataHelper.g.cpp"
#endif

#include <appmodel.h>
#include <filesystem>

namespace winrt::CelestiaAppComponent::implementation
{
    bool AppDataHelper::IsPackaged()
    {
        static const bool packaged = []
        {
            UINT32 length = 0;
            return ::GetCurrentPackageFullName(&length, nullptr) != APPMODEL_ERROR_NO_PACKAGE;
        }();
        return packaged;
    }

    Microsoft::Windows::Storage::ApplicationData AppDataHelper::Current()
    {
        static auto data = IsPackaged()
            ? Microsoft::Windows::Storage::ApplicationData::GetDefault()
            : Microsoft::Windows::Storage::ApplicationData::GetForUnpackaged(L"Celestia", L"Celestia");
        return data;
    }

    Microsoft::Windows::Storage::ApplicationDataContainer AppDataHelper::LocalSettings()
    {
        return Current().LocalSettings();
    }

    Windows::Storage::StorageFolder AppDataHelper::LocalFolder()
    {
        auto current = Current();
        try
        {
            return current.LocalFolder();
        }
        catch (hresult_error const& e)
        {
            if (e.code() != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                throw;
            std::filesystem::create_directories(std::filesystem::path(current.LocalPath().c_str()));
            return current.LocalFolder();
        }
    }

    Windows::Storage::StorageFolder AppDataHelper::TemporaryFolder()
    {
        auto current = Current();
        try
        {
            return current.TemporaryFolder();
        }
        catch (hresult_error const& e)
        {
            if (e.code() != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                throw;
            std::filesystem::create_directories(std::filesystem::path(current.TemporaryPath().c_str()));
            return current.TemporaryFolder();
        }
    }
}
