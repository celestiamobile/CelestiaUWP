// InfoUserControl.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "InfoUserControl.xaml.h"
#if __has_include("InfoUserControl.g.cpp")
#include "InfoUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    InfoUserControl::InfoUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, CelestiaSelection const& selection, bool preserveMargin) : appCore(appCore), renderer(renderer), selection(selection), preserveMargin(preserveMargin)
    {
    }

    void InfoUserControl::InitializeComponent()
    {
        InfoUserControlT::InitializeComponent();
        ControlStrip().AppCore(appCore);
        ControlStrip().Renderer(renderer);
        ControlStrip().Selection(selection);

        if (!preserveMargin)
        {
            Thickness margin{ 0, 0, 0, 0 };
            Container().Margin(margin);
        }

        CockpitCheckbox().Content(box_value(LocalizationHelper::Localize(L"Use as Cockpit", L"Option to use a spacecraft as cockpit")));
        auto body = selection.Object().try_as<CelestiaBody>();
        if (body != nullptr && body.CanBeUsedAsCockpit())
        {
            CockpitCheckbox().Visibility(Visibility::Visible);
            CockpitCheckbox().IsChecked(selection.Equals(appCore.Simulation().ActiveObserver().Cockpit()));
        }
        else
        {
            CockpitCheckbox().Visibility(Visibility::Collapsed);
        }

        auto obj = selection.Object();
        if (obj == nullptr) return;

        renderer.EnqueueTask([weak_this{ get_weak() }, obj]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                auto name = strong_this->appCore.Simulation().Universe().NameForSelection(strong_this->selection);
                auto detail = SelectionHelper::GetOverview(strong_this->selection, strong_this->appCore);
                auto url = strong_this->selection.InfoURL();
                strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [strong_this, name, detail, url]()
                    {
                        strong_this->NameLabel().Text(name);
                        strong_this->DetailLabel().Text(detail);
                        if (!url.empty())
                        {
                            strong_this->LinkButton().NavigateUri(Uri(url));
                            strong_this->LinkButton().Content(box_value(url));
                            strong_this->LinkButton().Visibility(Visibility::Visible);
                        }
                        else
                        {
                            strong_this->LinkButton().Visibility(Visibility::Collapsed);
                        }
                    });
            });
    }

    event_token InfoUserControl::ShowSubsystem(Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs> const& handler)
    {
        return showSubsystemEvent.add(handler);
    }

    void InfoUserControl::ShowSubsystem(event_token const& token) noexcept
    {
        showSubsystemEvent.remove(token);
    }

    void InfoUserControl::CockpitCheckbox_Checked(IInspectable const&, RoutedEventArgs const&)
    {
        renderer.EnqueueTask([weak_this{ get_weak() }]()
        {
            auto strong_this{ weak_this.get() };
            if (strong_this == nullptr) return;
            strong_this->appCore.Simulation().ActiveObserver().Cockpit(strong_this->selection);
        });
    }

    void InfoUserControl::CockpitCheckbox_Unchecked(IInspectable const&, RoutedEventArgs const&)
    {
        renderer.EnqueueTask([weak_this{ get_weak() }]()
        {
            auto strong_this{ weak_this.get() };
            if (strong_this == nullptr) return;
            strong_this->appCore.Simulation().ActiveObserver().Cockpit(CelestiaSelection());
        });
    }

    void InfoUserControl::ControlStrip_ShowSubsystem(IInspectable const&, CelestiaWinUI::InfoShowSubsystemArgs const& args)
    {
        showSubsystemEvent(*this, args);
    }
}