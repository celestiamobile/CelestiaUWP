// AppDataHelper.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "AppDataHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct AppDataHelper : AppDataHelperT<AppDataHelper>
    {
        static bool IsPackaged();
        static Microsoft::Windows::Storage::ApplicationData Current();
        static Microsoft::Windows::Storage::ApplicationDataContainer LocalSettings();
        static Windows::Storage::StorageFolder LocalFolder();
        static Windows::Storage::StorageFolder TemporaryFolder();
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct AppDataHelper : AppDataHelperT<AppDataHelper, implementation::AppDataHelper>
    {
    };
}
