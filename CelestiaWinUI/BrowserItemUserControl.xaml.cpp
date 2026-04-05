// BrowserItemUserControl.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "BrowserItemUserControl.xaml.h"
#if __has_include("BrowserItemUserControl.g.cpp")
#include "BrowserItemUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::CelestiaWinUI::implementation
{
    BrowserItemUserControl::BrowserItemUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, BrowserItemTab const& item, bool preserveMargin) : appCore(appCore), renderer(renderer), preserveMargin(preserveMargin)
    {
        rootItem = BrowserItem::ConvertToBindable(item.Children());
    }

    void BrowserItemUserControl::InitializeComponent()
    {
        BrowserItemUserControlT::InitializeComponent();
        ControlStrip().AppCore(appCore);
        ControlStrip().Renderer(renderer);

        if (!preserveMargin)
        {
            Thickness margin{ 0, 0, 0, 0 };
            Container().Margin(margin);
        }
    }

    Microsoft::UI::Xaml::Interop::IBindableObservableVector BrowserItemUserControl::RootItem()
    {
        return rootItem;
    }

    void BrowserItemUserControl::Tree_SelectionChanged(TreeView const&, TreeViewSelectionChangedEventArgs const& args)
    {
        auto selectedItem = Tree().SelectedItem();
        if (auto removedItems = args.RemovedItems(); removedItems != nullptr && removedItems.Size() > 0 && removedItems.GetAt(0) == selectedItem)
            selectedItem = nullptr;
        if (auto addedItems = args.AddedItems(); addedItems != nullptr && addedItems.Size() > 0)
            selectedItem = addedItems.GetAt(0);

        if (selectedItem == nullptr)
        {
            ControlStrip().Selection(nullptr);
            return;
        }
        auto browserItem = selectedItem.try_as<BrowserItem>();
        if (browserItem == nullptr)
        {
            ControlStrip().Selection(nullptr);
            return;
        }
        auto object = browserItem.Item().Object();
        if (object == nullptr)
        {
            ControlStrip().Selection(nullptr);
            return;
        }
        ControlStrip().Selection(CelestiaSelection(object));
    }

    event_token BrowserItemUserControl::GetInfo(EventHandler<CelestiaWinUI::InfoGetInfoArgs> const& handler)
    {
        return getInfoEvent.add(handler);
    }

    void BrowserItemUserControl::GetInfo(event_token const& token) noexcept
    {
        getInfoEvent.remove(token);
    }

    event_token BrowserItemUserControl::OpenURL(Windows::Foundation::EventHandler<hstring> const& handler)
    {
        return openURLEvent.add(handler);
    }

    void BrowserItemUserControl::OpenURL(event_token const& token) noexcept
    {
        openURLEvent.remove(token);
    }

    event_token BrowserItemUserControl::ShowSubsystem(Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs> const& handler)
    {
        return showSubsystemEvent.add(handler);
    }

    void BrowserItemUserControl::ShowSubsystem(event_token const& token) noexcept
    {
        showSubsystemEvent.remove(token);
    }

    void BrowserItemUserControl::ControlStrip_GetInfo(IInspectable const&, CelestiaWinUI::InfoGetInfoArgs const& args)
    {
        getInfoEvent(*this, args);
    }

    void BrowserItemUserControl::ControlStrip_ShowSubsystem(IInspectable const&, CelestiaWinUI::InfoShowSubsystemArgs const& args)
    {
        showSubsystemEvent(*this, args);
    }
}