//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaStarBrowser.h"
#if __has_include("CelestiaStarBrowser.g.cpp")
#include "CelestiaStarBrowser.g.cpp"
#endif

using namespace std;

#define BROWSER_MAX_STAR_COUNT          100

namespace winrt::CelestiaComponent::implementation
{
	CelestiaStarBrowser::CelestiaStarBrowser(StarBrowser* sb) : CelestiaStarBrowserT<CelestiaStarBrowser>(), sb(sb)
	{
	}

	com_array<CelestiaComponent::CelestiaStar> CelestiaStarBrowser::Stars()
	{
        std::vector<const Star*>* stars = sb->listStars(BROWSER_MAX_STAR_COUNT);
        if (stars == nullptr)
            return com_array<CelestiaComponent::CelestiaStar>();

        std::vector<CelestiaComponent::CelestiaStar> starVec;
        for (int i = 0; i < stars->size(); i++)
        {
            Star* aStar = (Star*)(*stars)[i];
            starVec.push_back(make<CelestiaStar>(aStar));
        }

        delete stars;
        return com_array<CelestiaComponent::CelestiaStar>(starVec);
	}

    void CelestiaStarBrowser::Close()
    {
        if (sb != nullptr)
        {
            delete sb;
            sb = nullptr;
        }
    }

	CelestiaStarBrowser::~CelestiaStarBrowser()
	{
        Close();
	}
}
