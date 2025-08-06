// CelestiaAstroObject.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaAstroObject.h"
#if __has_include("CelestiaAstroObject.g.cpp")
#include "CelestiaAstroObject.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaAstroObject::CelestiaAstroObject(void* obj) : CelestiaAstroObjectT<CelestiaAstroObject>(), obj(obj)
	{
	}
}
