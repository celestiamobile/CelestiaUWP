#pragma once

#include "LocalizationHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct LocalizationHelper : LocalizationHelperT<LocalizationHelper>
    {
        static hstring Locale();
        static void Locale(hstring const& locale);
        static hstring Localize(hstring const& original);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct LocalizationHelper : LocalizationHelperT<LocalizationHelper, implementation::LocalizationHelper>
    {
    };
}
