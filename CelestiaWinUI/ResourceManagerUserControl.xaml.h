#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "ResourceManagerUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct ResourceManagerUserControl : ResourceManagerUserControlT<ResourceManagerUserControl>
    {
        ResourceManagerUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaAppComponent::ResourceManager const& resourceManager, CelestiaWinUI::WebWindowProvider const& windowProvider, CelestiaWinUI::GetAddonHandler const& getAddonHandler);

        void Nav_BackRequested(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaAppComponent::ResourceManager resourceManager;
        CelestiaWinUI::WebWindowProvider windowProvider;
        CelestiaWinUI::ResourceItemUserControl currentItemControl{ nullptr };
        CelestiaWinUI::InstalledItemListUserControl installedList;

        void ShowItem(CelestiaAppComponent::ResourceItem const& item);
   };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct ResourceManagerUserControl : ResourceManagerUserControlT<ResourceManagerUserControl, implementation::ResourceManagerUserControl>
    {
    };
}
