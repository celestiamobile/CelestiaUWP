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
