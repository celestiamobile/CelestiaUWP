#include "pch.h"
#include "GuideItem.h"
#if __has_include("GuideItem.g.cpp")
#include "GuideItem.g.cpp"
#endif

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Data::Json;

namespace winrt::CelestiaAppComponent::implementation
{
    GuideItem::GuideItem(hstring const& title, hstring const& id) : title(title), id(id)
    {
    }

    hstring GuideItem::Title()
    {
        return title;
    }

    hstring GuideItem::ID()
    {
        return id;
    }

    CelestiaAppComponent::GuideItem GuideItem::TryParse(hstring const& data)
    {
        auto object = JsonObject::Parse(data);
        // Get mandatory fields
        auto title = object.GetNamedString(L"title");
        auto id = object.GetNamedString(L"id");
        return CelestiaAppComponent::GuideItem(title, id);
    }
}
