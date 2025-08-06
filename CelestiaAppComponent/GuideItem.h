// GuideItem.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "GuideItem.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct GuideItem : GuideItemT<GuideItem>
    {
        GuideItem(hstring const& title, hstring const& id);
        hstring Title();
        hstring ID();

        static CelestiaAppComponent::GuideItem TryParse(hstring const& data);

    private:
        hstring title;
        hstring id;
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct GuideItem : GuideItemT<GuideItem, implementation::GuideItem>
    {
    };
}
