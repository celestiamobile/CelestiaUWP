//
// CelestiaCompletion.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include <celengine/completion.h>
#include "CelestiaSelection.h"
#include "CelestiaCompletion.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaCompletion : CelestiaCompletionT<CelestiaCompletion>
    {
        CelestiaCompletion(celestia::engine::Completion const& completion);

        CelestiaComponent::CelestiaSelection Selection();
        hstring Name();

    private:
        celestia::engine::Completion completion;
    };
}
