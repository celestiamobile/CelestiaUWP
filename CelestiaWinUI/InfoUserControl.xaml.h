#pragma once

#include "InfoUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct InfoUserControl : InfoUserControlT<InfoUserControl>
    {
        InfoUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSelection const& selection);

    private:
        hstring GetBodyOverview(CelestiaComponent::CelestiaBody const& body, CelestiaComponent::CelestiaAppCore const& AppCore);
        hstring GetStarOverview(CelestiaComponent::CelestiaStar const& star, CelestiaComponent::CelestiaAppCore const& AppCore);
        hstring GetDSOOveriew(CelestiaComponent::CelestiaDSO const& dso, CelestiaComponent::CelestiaAppCore const& AppCore);
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct InfoUserControl : InfoUserControlT<InfoUserControl, implementation::InfoUserControl>
    {
    };
}
