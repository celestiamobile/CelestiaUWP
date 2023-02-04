#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "InstalledItemListItemSelectedArgs.g.h"
#include "InstalledItemListUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct InstalledItemListItemSelectedArgs : InstalledItemListItemSelectedArgsT<InstalledItemListItemSelectedArgs>
    {
        InstalledItemListItemSelectedArgs(CelestiaAppComponent::ResourceItem const& item) : item(item) {}
        CelestiaAppComponent::ResourceItem Item() { return item; }
    private:
        CelestiaAppComponent::ResourceItem item;
    };

    struct InstalledItemListUserControl : InstalledItemListUserControlT<InstalledItemListUserControl>
    {
        InstalledItemListUserControl(CelestiaAppComponent::ResourceManager const& resourceManager, CelestiaWinUI::GetAddonHandler const& getAddonHandler);

        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::ResourceItem> Items();

        event_token ItemSelected(Windows::Foundation::EventHandler<CelestiaWinUI::InstalledItemListItemSelectedArgs> const&);
        void ItemSelected(event_token const&);

        void Reload();

        void ItemList_ItemClick(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::ItemClickEventArgs const&);
        void GetAddonButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        CelestiaAppComponent::ResourceManager resourceManager;
        CelestiaWinUI::GetAddonHandler getAddonHandler;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::ResourceItem> items;
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InstalledItemListItemSelectedArgs>> itemSelectedEvent;

        fire_and_forget LoadItems();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct InstalledItemListUserControl : InstalledItemListUserControlT<InstalledItemListUserControl, implementation::InstalledItemListUserControl>
    {
    };
}
