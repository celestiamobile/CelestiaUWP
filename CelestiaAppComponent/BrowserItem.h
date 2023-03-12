#pragma once

#include "BrowserItem.g.h"
#include "BrowserItemTab.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct BrowserItem : BrowserItemT<BrowserItem>
    {
        BrowserItem(CelestiaComponent::CelestiaBrowserItem const& item);
        CelestiaComponent::CelestiaBrowserItem Item();
        hstring ItemName();
        bool IsExpanded();
        void IsExpanded(bool);
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItem> Children();
        Windows::UI::Xaml::Interop::IBindableObservableVector BindableChildren();
        static Windows::UI::Xaml::Interop::IBindableObservableVector ConvertToBindable(Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItem> const& list);

    private:
        CelestiaComponent::CelestiaBrowserItem item;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItem> children;
        bool areChildrenLoaded{ false };
        bool areBindableChildrenLoaded{ false };
        bool expanded{ false };
        Windows::UI::Xaml::Interop::IBindableObservableVector bindableChildren;
    };

    struct BrowserItemTab : BrowserItemTabT<BrowserItemTab>
    {
        BrowserItemTab(Windows::Foundation::Collections::IVector<CelestiaAppComponent::BrowserItem> const& children, hstring const& tabName);
        hstring TabName();
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItem> Children();

    private:
        hstring tabName;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItem> children;
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct BrowserItem : BrowserItemT<BrowserItem, implementation::BrowserItem>
    {
    };

    struct BrowserItemTab : BrowserItemTabT<BrowserItemTab, implementation::BrowserItemTab>
    {
    };
}
