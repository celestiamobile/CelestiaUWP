#include "pch.h"
#include "ResourceManagerUserControl.xaml.h"
#if __has_include("ResourceManagerUserControl.g.cpp")
#include "ResourceManagerUserControl.g.cpp"
#endif

#include <winrt/Windows.UI.Xaml.Interop.h>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;

namespace winrt::CelestiaWinUI::implementation
{
    ResourceManagerUserControl::ResourceManagerUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, ResourceManager const& resourceManager, CelestiaWinUI::WebWindowProvider const& windowProvider) : appCore(appCore), renderer(renderer), resourceManager(resourceManager), windowProvider(windowProvider)
    {
        InitializeComponent();

        InstalledItemListUserControl installedList{ resourceManager };
        installedList.ItemSelected([weak_this{ get_weak() }](IInspectable const&, InstalledItemListItemSelectedArgs const& args)
            {
                auto strong_this { weak_this.get() };
                if (!strong_this) return;
                strong_this->ShowItem(args.Item());
            });

        Container().Navigate(xaml_typename<CelestiaWinUI::CustomPage>(), installedList);
    }

    void ResourceManagerUserControl::Nav_BackRequested(IInspectable const&, Controls::NavigationViewBackRequestedEventArgs const&)
    {
        if (!Container().CanGoBack()) return;

        if (currentItemControl != nullptr)
        {
            currentItemControl.CloseWebViewIfNeeded();
            currentItemControl = nullptr;
        }
        Container().GoBack();
        Nav().IsBackEnabled(Container().CanGoBack());
    }

    void ResourceManagerUserControl::ShowItem(CelestiaAppComponent::ResourceItem const& item)
    {
        if (currentItemControl != nullptr)
        {
            currentItemControl.CloseWebViewIfNeeded();
            currentItemControl = nullptr;
        }
        ResourceItemUserControl userControl{ appCore, renderer, item, resourceManager, windowProvider };
        Container().Navigate(xaml_typename<CelestiaWinUI::CustomPage>(), userControl);
        Nav().IsBackEnabled(true);
        currentItemControl = userControl;
    }
}