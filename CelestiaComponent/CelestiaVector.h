// CelestiaVector.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <vector>
#include "CelestiaVector.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaVector : CelestiaVectorT<CelestiaVector>
    {
        CelestiaVector(double x, double y, double z, double w);
        CelestiaVector(double x, double y, double z);

        double X();
        double Y();
        double Z();
        double W();

        std::vector<double> array;
    };
}