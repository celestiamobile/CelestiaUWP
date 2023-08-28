#pragma once

#include "SearchObjectEntry.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct SearchObjectEntry : SearchObjectEntryT<SearchObjectEntry>
    {
        SearchObjectEntry(hstring const& name, hstring const& path);
        hstring Name();
        hstring Path();

    private:
        hstring name;
        hstring path;
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct SearchObjectEntry : SearchObjectEntryT<SearchObjectEntry, implementation::SearchObjectEntry>
    {
    };
}
