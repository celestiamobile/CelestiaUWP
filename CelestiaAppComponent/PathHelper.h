#pragma once

#include "PathHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct PathHelper : PathHelperT<PathHelper>
    {
        static hstring Combine(hstring const& path1, hstring const& path2);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct PathHelper : PathHelperT<PathHelper, implementation::PathHelper>
    {
    };
}
