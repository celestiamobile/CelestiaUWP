// BrowserItem.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "BrowserItem.h"
#if __has_include("BrowserItem.g.cpp")
#include "BrowserItem.g.cpp"
#endif
#if __has_include("BrowserItemTab.g.cpp")
#include "BrowserItemTab.g.cpp"
#endif
#if __has_include("BrowserGetInfoAction.g.cpp")
#include "BrowserGetInfoAction.g.cpp"
#endif
#if __has_include("BrowserShowSubsystemAction.g.cpp")
#include "BrowserShowSubsystemAction.g.cpp"
#endif
#if __has_include("BrowserMarkMenuItem.g.cpp")
#include "BrowserMarkMenuItem.g.cpp"
#endif
#if __has_include("BrowserMarkAction.g.cpp")
#include "BrowserMarkAction.g.cpp"
#endif
#if __has_include("BrowserInputAction.g.cpp")
#include "BrowserInputAction.g.cpp"
#endif

#include "ObservableVector.h"

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::CelestiaAppComponent::implementation
{
    BrowserGetInfoAction::BrowserGetInfoAction()
    {
    }

    hstring BrowserGetInfoAction::Name()
    {
        return LocalizationHelper::Localize(L"Get Info", L"Action for getting info about current selected object");
    }

    BrowserShowSubsystemAction::BrowserShowSubsystemAction()
    {
    }

    hstring BrowserShowSubsystemAction::Name()
    {
        return LocalizationHelper::Localize(L"Subsystem", L"Subsystem of an object (e.g. planetarium system)");
    }

    BrowserInputAction::BrowserInputAction(hstring const& name, int16_t code) : name(name), code(code)
    {
    }

    hstring BrowserInputAction::Name()
    {
        return name;
    }

    int16_t BrowserInputAction::Code()
    {
        return code;
    }

    BrowserMarkMenuItem::BrowserMarkMenuItem(bool mark, CelestiaComponent::CelestiaMarkerRepresentation marker) : mark(mark), marker(marker)
    {
    }

    hstring BrowserMarkMenuItem::Title()
    {
        using namespace CelestiaComponent;
        if (!mark)
            return LocalizationHelper::Localize(L"Unmark", L"Unmark an object");
        switch (marker)
        {
        case CelestiaMarkerRepresentation::Diamond:
            return LocalizationHelper::Localize(L"Diamond", L"Marker");
        case CelestiaMarkerRepresentation::Triangle:
            return LocalizationHelper::Localize(L"Triangle", L"Marker");
        case CelestiaMarkerRepresentation::Square:
            return LocalizationHelper::Localize(L"Square", L"Marker");
        case CelestiaMarkerRepresentation::FilledSquare:
            return LocalizationHelper::Localize(L"Filled Square", L"Marker");
        case CelestiaMarkerRepresentation::Plus:
            return LocalizationHelper::Localize(L"Plus", L"Marker");
        case CelestiaMarkerRepresentation::X:
            return LocalizationHelper::Localize(L"X", L"Marker");
        case CelestiaMarkerRepresentation::LeftArrow:
            return LocalizationHelper::Localize(L"Left Arrow", L"Marker");
        case CelestiaMarkerRepresentation::RightArrow:
            return LocalizationHelper::Localize(L"Right Arrow", L"Marker");
        case CelestiaMarkerRepresentation::UpArrow:
            return LocalizationHelper::Localize(L"Up Arrow", L"Marker");
        case CelestiaMarkerRepresentation::DownArrow:
            return LocalizationHelper::Localize(L"Down Arrow", L"Marker");
        case CelestiaMarkerRepresentation::Circle:
            return LocalizationHelper::Localize(L"Circle", L"Marker");
        case CelestiaMarkerRepresentation::Disk:
            return LocalizationHelper::Localize(L"Disk", L"Marker");
        case CelestiaMarkerRepresentation::Crosshair:
            return LocalizationHelper::Localize(L"Crosshair", L"Marker");
        }
        return L"";
    }

    bool BrowserMarkMenuItem::Mark()
    {
        return mark;
    }

    CelestiaComponent::CelestiaMarkerRepresentation BrowserMarkMenuItem::Marker()
    {
        return marker;
    }

    BrowserMarkAction::BrowserMarkAction()
    {
        using namespace CelestiaComponent;
        menuItems = single_threaded_observable_vector<CelestiaAppComponent::BrowserMarkMenuItem>
        ({
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::Diamond),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::Triangle),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::Square),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::FilledSquare),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::Plus),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::X),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::LeftArrow),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::RightArrow),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::UpArrow),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::DownArrow),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::Circle),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::Disk),
            CelestiaAppComponent::BrowserMarkMenuItem(true, CelestiaMarkerRepresentation::Crosshair),
            CelestiaAppComponent::BrowserMarkMenuItem(false, CelestiaMarkerRepresentation::Diamond),
        });
    }

    hstring BrowserMarkAction::Name()
    {
        return LocalizationHelper::Localize(L"Mark", L"Mark an object");
    }

    Collections::IObservableVector<CelestiaAppComponent::BrowserMarkMenuItem> BrowserMarkAction::MenuItems()
    {
        return menuItems;
    }

    BrowserItem::BrowserItem(CelestiaComponent::CelestiaBrowserItem const& item) : item(item)
    {
        children = single_threaded_observable_vector<CelestiaAppComponent::BrowserItem>();
    }

    CelestiaComponent::CelestiaBrowserItem BrowserItem::Item() { return item; }

    bool BrowserItem::IsExpanded()
    {
        return expanded;
    }

    void BrowserItem::IsExpanded(bool value)
    {
        expanded = value;
    }

    hstring BrowserItem::ItemName()
    {
        return item.Name();
    }

    Collections::IObservableVector<CelestiaAppComponent::BrowserItem> BrowserItem::Children()
    {
        if (!areChildrenLoaded)
        {
            for (const auto& child : item.Children())
            {
                children.Append(CelestiaAppComponent::BrowserItem(child));
            }
            areChildrenLoaded = true;
        }
        return children;
    }

    Microsoft::UI::Xaml::Interop::IBindableObservableVector BrowserItem::BindableChildren()
    {
        if (!areBindableChildrenLoaded)
        {
            bindableChildren = ConvertToBindable(Children());
            areBindableChildrenLoaded = true;
        }
        return bindableChildren;
    }

    Microsoft::UI::Xaml::Interop::IBindableObservableVector BrowserItem::ConvertToBindable(Collections::IObservableVector<CelestiaAppComponent::BrowserItem> const& list)
    {
        return make<ObservableVector<CelestiaAppComponent::BrowserItem>>(list);
    }

    BrowserItemTab::BrowserItemTab(Collections::IVector<CelestiaAppComponent::BrowserItem> const& children, hstring const& tabName) : tabName(tabName)
    {
        this->children = single_threaded_observable_vector<CelestiaAppComponent::BrowserItem>({ children.begin(), children.end() });
    }

    hstring BrowserItemTab::TabName()
    {
        return tabName;
    }

    Collections::IObservableVector<CelestiaAppComponent::BrowserItem> BrowserItemTab::Children()
    {
        return children;
    }
}
