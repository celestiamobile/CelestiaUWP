#pragma once

#include "ResourceItem.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct ResourceItem : ResourceItemT<ResourceItem>
    {
        ResourceItem(hstring const& name, hstring const& id, hstring const& url, hstring const& type, hstring const& demoObjectName, hstring const& mainScriptName);
        hstring Name();
        hstring ID();
        hstring URL();
        hstring Type();
        hstring DemoObjectName();
        hstring MainScriptName();

        static CelestiaAppComponent::ResourceItem TryParse(hstring const& data);

        Windows::Data::Json::JsonObject JSONRepresentation();

    private:
        hstring name;
        hstring id;
        hstring url;
        hstring type;
        hstring demoObjectName;
        hstring mainScriptName;
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct ResourceItem : ResourceItemT<ResourceItem, implementation::ResourceItem>
    {
    };
}
