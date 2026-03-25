// CelestiaLogger.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "CelestiaLogger.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaLogger : CelestiaLoggerT<CelestiaLogger>
    {
        static void SetLogFilePath(hstring const& path);
        static void Log(hstring const& message);
        static void Close();
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaLogger : CelestiaLoggerT<CelestiaLogger, implementation::CelestiaLogger>
    {
    };
}
