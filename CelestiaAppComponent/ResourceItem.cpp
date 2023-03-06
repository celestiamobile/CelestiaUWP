#include "pch.h"
#include "ResourceItem.h"
#if __has_include("ResourceItem.g.cpp")
#include "ResourceItem.g.cpp"
#endif

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Data::Json;

namespace winrt::CelestiaAppComponent::implementation
{
    ResourceItem::ResourceItem(hstring const& name, hstring const& id, hstring const& url, hstring const& type, hstring const& demoObjectName, hstring const& mainScriptName) : name(name), id(id), url(url), type(type), demoObjectName(demoObjectName), mainScriptName(mainScriptName)
    {
    }

    hstring ResourceItem::Name()
    {
        return name;
    }

    hstring ResourceItem::ID()
    {
        return id;
    }

    hstring ResourceItem::URL()
    {
        return url;
    }

    hstring ResourceItem::Type()
    {
        return type;
    }

    hstring ResourceItem::DemoObjectName()
    {
        return demoObjectName;
    }

    hstring ResourceItem::MainScriptName()
    {
        return mainScriptName;
    }

    CelestiaAppComponent::ResourceItem ResourceItem::TryParse(hstring const& data)
    {
        auto object = JsonObject::Parse(data);
        // Get mandatory fields
        auto name = object.GetNamedString(L"name");
        auto id = object.GetNamedString(L"id");
        auto url = object.GetNamedString(L"item");
        auto type = object.GetNamedString(L"type");
        // Get optional fields
        auto demoObjectName = object.GetNamedString(L"objectName", L"");
        auto mainScriptName = object.GetNamedString(L"mainScriptName", L"");
        return CelestiaAppComponent::ResourceItem(name, id, url, type, demoObjectName, mainScriptName);
    }

    JsonObject ResourceItem::JSONRepresentation()
    {
        JsonObject object;
        object.SetNamedValue(L"name", JsonValue::CreateStringValue(name));
        object.SetNamedValue(L"id", JsonValue::CreateStringValue(id));
        object.SetNamedValue(L"item", JsonValue::CreateStringValue(url));
        object.SetNamedValue(L"type", JsonValue::CreateStringValue(type));
        if (!demoObjectName.empty())
            object.SetNamedValue(L"objectName", JsonValue::CreateStringValue(demoObjectName));
        if (!mainScriptName.empty())
            object.SetNamedValue(L"mainScriptName", JsonValue::CreateStringValue(mainScriptName));
        return object;
    }
}
