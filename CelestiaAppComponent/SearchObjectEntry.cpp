#include "pch.h"
#include "SearchObjectEntry.h"
#if __has_include("SearchObjectEntry.g.cpp")
#include "SearchObjectEntry.g.cpp"
#endif

using namespace std;
using namespace winrt;

namespace winrt::CelestiaAppComponent::implementation
{
    SearchObjectEntry::SearchObjectEntry(hstring const& name, hstring const& path) : name(name), path(path)
    {
    }

    hstring SearchObjectEntry::Name()
    {
        return name;
    }

    hstring SearchObjectEntry::Path()
    {
        return path;
    }
}
