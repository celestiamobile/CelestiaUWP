#pragma once

#include "BrowserItem.g.h"
#include "BrowserItemTab.g.h"
#include "BrowserAction.g.h"
#include "BrowserMarkAction.g.h"
#include "BrowserMarkMenuItem.g.h"
#include "BrowserGetInfoAction.g.h"
#include "BrowserShowSubsystemAction.g.h"
#include "BrowserInputAction.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct BrowserAction : BrowserActionT<BrowserAction>
    {
        virtual hstring Name() = 0;
    };

    struct BrowserGetInfoAction : BrowserGetInfoActionT<BrowserGetInfoAction, BrowserAction>
    {
        BrowserGetInfoAction();
        hstring Name();
    };

    struct BrowserShowSubsystemAction : BrowserShowSubsystemActionT<BrowserShowSubsystemAction, BrowserAction>
    {
        BrowserShowSubsystemAction();
        hstring Name();
    };

    struct BrowserMarkMenuItem : BrowserMarkMenuItemT<BrowserMarkMenuItem>
    {
        BrowserMarkMenuItem(bool mark, CelestiaComponent::CelestiaMarkerRepresentation marker);
        hstring Title();
        bool Mark();
        CelestiaComponent::CelestiaMarkerRepresentation Marker();

    private:
        bool mark;
        CelestiaComponent::CelestiaMarkerRepresentation marker;
    };

    struct BrowserMarkAction : BrowserMarkActionT<BrowserMarkAction, BrowserAction>
    {
        BrowserMarkAction();
        hstring Name();
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserMarkMenuItem> MenuItems();

    private:
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserMarkMenuItem> menuItems;
    };

    struct BrowserInputAction : BrowserInputActionT<BrowserInputAction, BrowserAction>
    {
        BrowserInputAction(hstring const& name, int16_t code);
        hstring Name();
        int16_t Code();

    private:
        hstring name;
        int16_t code;
    };

    struct BrowserItem : BrowserItemT<BrowserItem>
    {
        BrowserItem(CelestiaComponent::CelestiaBrowserItem const& item);
        CelestiaComponent::CelestiaBrowserItem Item();
        hstring ItemName();
        bool IsExpanded();
        void IsExpanded(bool);
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItem> Children();
        Microsoft::UI::Xaml::Interop::IBindableObservableVector BindableChildren();
        static Microsoft::UI::Xaml::Interop::IBindableObservableVector ConvertToBindable(Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItem> const& list);

    private:
        CelestiaComponent::CelestiaBrowserItem item;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItem> children;
        bool areChildrenLoaded{ false };
        bool areBindableChildrenLoaded{ false };
        bool expanded{ false };
        Microsoft::UI::Xaml::Interop::IBindableObservableVector bindableChildren;
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
    struct BrowserGetInfoAction : BrowserGetInfoActionT<BrowserGetInfoAction, implementation::BrowserGetInfoAction>
    {
    };

    struct BrowserShowSubsystemAction : BrowserShowSubsystemActionT<BrowserShowSubsystemAction, implementation::BrowserShowSubsystemAction>
    {
    };

    struct BrowserMarkAction : BrowserMarkActionT<BrowserMarkAction, implementation::BrowserMarkAction>
    {
    };

    struct BrowserInputAction : BrowserInputActionT<BrowserInputAction, implementation::BrowserInputAction>
    {
    };

    struct BrowserItem : BrowserItemT<BrowserItem, implementation::BrowserItem>
    {
    };

    struct BrowserItemTab : BrowserItemTabT<BrowserItemTab, implementation::BrowserItemTab>
    {
    };

    struct BrowserMarkMenuItem : BrowserMarkMenuItemT<BrowserMarkMenuItem, implementation::BrowserMarkMenuItem>
    {
    };
}
