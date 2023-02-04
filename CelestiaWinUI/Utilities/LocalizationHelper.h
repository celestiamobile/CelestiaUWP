#pragma once

#include "LocalizationHelper.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct LocalizationHelper : LocalizationHelperT<LocalizationHelper>
    {
        static hstring Locale();
        static void Locale(hstring const& locale);
        static hstring Localize(hstring const& original);
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct LocalizationHelper : LocalizationHelperT<LocalizationHelper, implementation::LocalizationHelper>
    {
    };
}
