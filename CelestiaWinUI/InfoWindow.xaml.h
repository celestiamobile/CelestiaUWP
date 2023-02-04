#pragma once

#include "InfoWindow.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct InfoWindow : InfoWindowT<InfoWindow>
    {
        InfoWindow(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSelection const& selection);

    private:
        hstring GetBodyOverview(CelestiaComponent::CelestiaBody const& body, CelestiaComponent::CelestiaAppCore const& AppCore);
        hstring GetStarOverview(CelestiaComponent::CelestiaStar const& star, CelestiaComponent::CelestiaAppCore const& AppCore);
        hstring GetDSOOveriew(CelestiaComponent::CelestiaDSO const& dso, CelestiaComponent::CelestiaAppCore const& AppCore);
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct InfoWindow : InfoWindowT<InfoWindow, implementation::InfoWindow>
    {
    };
}
