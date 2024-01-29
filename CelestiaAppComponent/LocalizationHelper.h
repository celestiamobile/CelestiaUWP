#pragma once

#include "LocalizationHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct LocalizationHelper : LocalizationHelperT<LocalizationHelper>
    {
        static hstring Locale();
        static void Locale(hstring const& locale);
        static hstring Localize(hstring const& original, hstring const& comment);
        static hstring Localize(hstring const& original, hstring const& context, hstring const& comment);
        static hstring FromWindowsTag(hstring const& tag);
        static hstring ToWindowsTag(hstring const& locale);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct LocalizationHelper : LocalizationHelperT<LocalizationHelper, implementation::LocalizationHelper>
    {
    };
}
