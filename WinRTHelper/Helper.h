#pragma once

#include "Helper.g.h"

namespace winrt::WinRTHelper::implementation
{
    struct Helper : HelperT<Helper>
    {
        static void PropertySetAddSize(Windows::Foundation::Collections::PropertySet const& propertySet, hstring const& key, Windows::Foundation::Size const& value);
        static void PropertySetAddSingle(Windows::Foundation::Collections::PropertySet const& propertySet, hstring const& key, float value);
    };
}

namespace winrt::WinRTHelper::factory_implementation
{
    struct Helper : HelperT<Helper, implementation::Helper>
    {
    };
}
