// CelestiaStarBrowser.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaStarBrowser.h"
#if __has_include("CelestiaStarBrowser.g.cpp")
#include "CelestiaStarBrowser.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaStarBrowser::CelestiaStarBrowser(std::unique_ptr<celestia::engine::StarBrowser> sb) : CelestiaStarBrowserT<CelestiaStarBrowser>(), sb(std::move(sb))
	{
	}

	com_array<CelestiaComponent::CelestiaStar> CelestiaStarBrowser::Stars()
	{
        std::vector<celestia::engine::StarBrowserRecord> records;
        sb->populate(records);
        if (records.empty())
            return com_array<CelestiaComponent::CelestiaStar>();

        std::vector<CelestiaComponent::CelestiaStar> starVec;
        for (const auto &record : records)
            starVec.emplace_back(make<CelestiaStar>(const_cast<Star*>(record.star)));

        return com_array<CelestiaComponent::CelestiaStar>(starVec);
	}

    void CelestiaStarBrowser::Close()
    {
        sb.reset();
    }

	CelestiaStarBrowser::~CelestiaStarBrowser()
	{
        Close();
	}
}
