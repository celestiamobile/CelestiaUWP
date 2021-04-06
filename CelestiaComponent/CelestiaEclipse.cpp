//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaHelper.h"
#include "CelestiaEclipse.h"
#if __has_include("CelestiaEclipse.g.cpp")
#include "CelestiaEclipse.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaEclipse::CelestiaEclipse(Eclipse* e) : CelestiaEclipseT<CelestiaEclipse>(), startTime(e->startTime), endTime(e->endTime), occulter(nullptr), receiver(nullptr)
	{
		occulter = make<CelestiaBody>(e->occulter);
		receiver = make<CelestiaBody>(e->receiver);
	}

	CelestiaComponent::CelestiaBody CelestiaEclipse::Occulter()
	{
		return occulter;
	}

	CelestiaComponent::CelestiaBody CelestiaEclipse::Receiver()
	{
		return receiver;
	}

	Windows::Foundation::DateTime CelestiaEclipse::StartTime()
	{
		return CelestiaHelper::DateTimeFromJulianDay(startTime);
	}

	Windows::Foundation::DateTime CelestiaEclipse::EndTime()
	{
		return CelestiaHelper::DateTimeFromJulianDay(endTime);
	}
}
