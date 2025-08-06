// CelestiaVector.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaVector.h"
#if __has_include("CelestiaVector.g.cpp")
#include "CelestiaVector.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaVector::CelestiaVector(double x, double y, double z, double w) : CelestiaVectorT<CelestiaVector>()
    {
        array.push_back(x);
        array.push_back(y);
        array.push_back(z);
        array.push_back(w);
    }

    CelestiaVector::CelestiaVector(double x, double y, double z) : CelestiaVectorT<CelestiaVector>()
    {
        array.push_back(x);
        array.push_back(y);
        array.push_back(z);
    }

    double CelestiaVector::X()
    {
        return array[0];
    }

    double CelestiaVector::Y()
    {
        return array[1];
    }

    double CelestiaVector::Z()
    {
        return array[2];
    }

    double CelestiaVector::W()
    {
        return array[3];
    }
}
