// InfoControlStrip.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "InfoControlStrip.xaml.h"
#if __has_include("InfoTemplateSelector.g.cpp")
#include "InfoTemplateSelector.g.cpp"
#endif
#if __has_include("InfoControlStrip.g.cpp")
#include "InfoControlStrip.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    InfoTemplateSelector::InfoTemplateSelector()
    {
    }

    DataTemplate InfoTemplateSelector::Action()
    {
        return action;
    }

    void InfoTemplateSelector::Action(DataTemplate const& value)
    {
        action = value;
    }

    DataTemplate InfoTemplateSelector::DropDown()
    {
        return dropDown;
    }

    void InfoTemplateSelector::DropDown(DataTemplate const& value)
    {
        dropDown = value;
    }

    DataTemplate InfoTemplateSelector::SelectTemplateCore(IInspectable const& item, DependencyObject const&)
    {
        return SelectTemplateCore(item);
    }

    DataTemplate InfoTemplateSelector::SelectTemplateCore(IInspectable const& item)
    {
        if (item.try_as<BrowserMarkAction>() != nullptr) return dropDown;
        if (item.try_as<BrowserAction>() != nullptr) return action;
        return nullptr;
    }

    InfoControlStrip::InfoControlStrip() : showsGetInfo(false), appCore(nullptr), renderer(nullptr), selection(nullptr)
    {
        actions = single_threaded_observable_vector<BrowserAction>
        ({
            BrowserInputAction(LocalizationHelper::Localize(L"Go", L"Go to an object"), 103),
            BrowserInputAction(LocalizationHelper::Localize(L"Center", L"Center an object"), 99),
            BrowserInputAction(LocalizationHelper::Localize(L"Follow", L""), 102),
            BrowserInputAction(LocalizationHelper::Localize(L"Sync Orbit", L""), 121),
            BrowserInputAction(LocalizationHelper::Localize(L"Lock Phase", L""), 58),
            BrowserInputAction(LocalizationHelper::Localize(L"Chase", L""), 34),
            BrowserInputAction(LocalizationHelper::Localize(L"Track", L"Track an object"), 116),
            BrowserShowSubsystemAction(),
            BrowserMarkAction(),
        });
    }

    CelestiaAppCore InfoControlStrip::AppCore()
    {
        return appCore;
    }

    CelestiaRenderer InfoControlStrip::Renderer()
    {
        return renderer;
    }
    CelestiaSelection InfoControlStrip::Selection()
    {
        return selection;
    }
    void InfoControlStrip::AppCore(CelestiaAppCore const& value)
    {
        appCore = value;
    }
    void InfoControlStrip::Renderer(CelestiaRenderer const& value)
    {
        renderer = value;
    }
    void InfoControlStrip::Selection(CelestiaSelection const& value)
    {
        selection = value;
    }

    bool InfoControlStrip::ShowsGetInfo()
    {
        return showsGetInfo;
    }

    void InfoControlStrip::ShowsGetInfo(bool value)
    {
        if (value != showsGetInfo)
        {
            if (value)
            {
                if (actions.Size() == 0 || actions.GetAt(0).try_as<BrowserGetInfoAction>() == nullptr)
                    actions.InsertAt(0, BrowserGetInfoAction());
            }
            else
            {
                if (actions.Size() > 0 && actions.GetAt(0).try_as<BrowserGetInfoAction>() != nullptr)
                    actions.RemoveAt(0);
            }
            showsGetInfo = value;
        }
    }

    Collections::IObservableVector<BrowserAction> InfoControlStrip::Actions()
    {
        return actions;
    }

    void InfoControlStrip::MarkButton_Loaded(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto button = sender.as<DropDownButton>();
        auto action = button.DataContext().as<BrowserMarkAction>();
        auto menuFlyout = button.Flyout().as<MenuFlyout>();
        menuFlyout.Items().Clear();

        for (const auto& menuItem : action.MenuItems())
        {
            MenuFlyoutItem menuFlyoutItem;
            menuFlyoutItem.DataContext(menuItem);
            menuFlyoutItem.Text(menuItem.Title());
            menuFlyoutItem.Click({ get_weak(), &InfoControlStrip::MarkMenuFlyoutItem_Click });
            menuFlyout.Items().Append(menuFlyoutItem);
        }
    }

    void InfoControlStrip::MarkMenuFlyoutItem_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        if (selection == nullptr || selection.IsEmpty())
            return;
        auto menuItem = sender.as<MenuFlyoutItem>().DataContext().as<BrowserMarkMenuItem>();
        renderer.EnqueueTask([weak_this{ get_weak() }, menuItem]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                if (!menuItem.Mark())
                {
                    strong_this->appCore.Simulation().Universe().UnmarkSelection(strong_this->selection);
                }
                else
                {
                    strong_this->appCore.Simulation().Universe().MarkSelection(strong_this->selection, menuItem.Marker());
                    strong_this->appCore.ShowMarkers(true);
                }
            });
    }

    void InfoControlStrip::ActionButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        if (selection == nullptr || selection.IsEmpty())
            return;
        auto action = sender.as<Button>().DataContext().as<BrowserAction>();
        if (action.try_as<BrowserShowSubsystemAction>() != nullptr)
        {
            auto args = make<InfoShowSubsystemArgs>(selection);
            showSubsystemEvent(*this, args);
            if (!args.Handled())
            {
                CelestiaAppCore core = appCore;
                auto items = single_threaded_observable_vector<BrowserItem>();
                CelestiaBrowserItem browserItem{ appCore.Simulation().Universe().NameForSelection(selection), selection.Object(), [core](CelestiaBrowserItem const& item)
                    {
                        return CelestiaExtension::GetChildren(item, core);
                    }, false };
                items.Append(BrowserItem(browserItem));
                BrowserItemUserControl userControl{ appCore, renderer, BrowserItemTab(items, L"") };
                Window window;
                window.SystemBackdrop(Media::MicaBackdrop());
                window.Content(userControl);
                window.Title(appCore.Simulation().Universe().NameForSelection(selection));
                WindowHelper::SetWindowIcon(window);
                WindowHelper::SetWindowTheme(window);
                WindowHelper::SetWindowFlowDirection(window);
                WindowHelper::ResizeWindow(window, 600, 600);
                window.Activate();
            }
        }
        else if (auto inputAction = action.try_as<BrowserInputAction>(); inputAction != nullptr)
        {
            renderer.EnqueueTask([weak_this{ get_weak() }, inputAction]()
                {
                    auto strong_this{ weak_this.get() };
                    if (strong_this == nullptr) return;
                    strong_this->appCore.Simulation().Selection(strong_this->selection);
                    strong_this->appCore.CharEnter(inputAction.Code());
                });
        }
        else if (auto getInfoAction = action.try_as<BrowserGetInfoAction>(); getInfoAction != nullptr)
        {
            auto args = make<InfoGetInfoArgs>(selection);
            getInfoEvent(*this, args);
            if (!args.Handled())
            {
                InfoUserControl userControl{ appCore, renderer, selection };
                Window window;
                window.SystemBackdrop(Media::MicaBackdrop());
                window.Content(userControl);
                window.Title(appCore.Simulation().Universe().NameForSelection(selection));
                WindowHelper::SetWindowIcon(window);
                WindowHelper::SetWindowTheme(window);
                WindowHelper::SetWindowFlowDirection(window);
                WindowHelper::ResizeWindow(window, 600, 600);
                window.Activate();
            }
        }
    }

    event_token InfoControlStrip::ShowSubsystem(EventHandler<CelestiaWinUI::InfoShowSubsystemArgs> const& handler)
    {
        return showSubsystemEvent.add(handler);
    }

    void InfoControlStrip::ShowSubsystem(event_token const& token) noexcept
    {
        showSubsystemEvent.remove(token);
    }

    event_token InfoControlStrip::GetInfo(EventHandler<CelestiaWinUI::InfoGetInfoArgs> const& handler)
    {
        return getInfoEvent.add(handler);
    }

    void InfoControlStrip::GetInfo(event_token const& token) noexcept
    {
        getInfoEvent.remove(token);
    }
}
