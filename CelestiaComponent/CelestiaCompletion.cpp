//
// CelestiaCompletion.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaCompletion.h"
#if __has_include("CelestiaCompletion.g.cpp")
#include "CelestiaCompletion.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaCompletion::CelestiaCompletion(celestia::engine::Completion const& completion) : completion(completion)
    {
    }

    CelestiaComponent::CelestiaSelection CelestiaCompletion::Selection()
    {
        return make<CelestiaSelection>(completion.getSelection());
    }

    hstring CelestiaCompletion::Name()
    {
        return to_hstring(completion.getName());
    }
}
