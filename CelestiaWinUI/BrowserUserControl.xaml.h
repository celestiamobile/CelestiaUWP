
#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "BrowserUserControl.g.h"
#include "BrowserItem.g.h"
#include "BrowserItemTab.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct BrowserItem : BrowserItemT<BrowserItem>
    {
        BrowserItem(CelestiaComponent::CelestiaBrowserItem const& item);
        CelestiaComponent::CelestiaBrowserItem Item();
        hstring ItemName();
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::BrowserItem> Children();

    private:
        CelestiaComponent::CelestiaBrowserItem item;
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::BrowserItem> children;
    };

    struct BrowserItemTab : BrowserItemTabT<BrowserItemTab>
    {
        BrowserItemTab(Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::BrowserItem> const& children, hstring const& tabName);
        hstring TabName();
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::BrowserItem> Children();

    private:
        hstring tabName;
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::BrowserItem> children;
    };

    struct BrowserUserControl : BrowserUserControlT<BrowserUserControl>
    {
        BrowserUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::BrowserItemTab> RootItems();
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::BrowserItem> RootItem();

        void NavigationView_SelectionChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::BrowserItemTab> rootItems;
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::BrowserItem> rootItem{ nullptr };

        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChangedEvent;

        void SetUpActions();
        void LoadData();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct BrowserUserControl : BrowserUserControlT<BrowserUserControl, implementation::BrowserUserControl>
    {
    };

    struct BrowserItem : BrowserItemT<BrowserItem, implementation::BrowserItem>
    {
    };

    struct BrowserItemTab : BrowserItemTabT<BrowserItemTab, implementation::BrowserItemTab>
    {
    };
}
