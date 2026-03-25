// CelestiaLogger.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaLogger.h"
#if __has_include("CelestiaLogger.g.cpp")
#include "CelestiaLogger.g.cpp"
#endif

#include <cstdio>

static FILE* logFile = nullptr;

namespace winrt::CelestiaComponent::implementation
{
    void CelestiaLogger::SetLogFilePath(hstring const& path)
    {
        logFile = _wfopen(path.c_str(), L"w");
    }

    void CelestiaLogger::Log(hstring const& message)
    {
        if (logFile == nullptr)
            return;

        fprintf(logFile, "%ls\n", message.c_str());
        fflush(logFile);
    }

    void CelestiaLogger::Close()
    {
        if (logFile == nullptr)
            return;

        fclose(logFile);
        logFile = nullptr;
    }
}
