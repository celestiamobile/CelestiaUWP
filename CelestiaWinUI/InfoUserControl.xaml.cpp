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
                auto timeLinks = SelectionHelper::GetTimeLinks(strong_this->selection, strong_this->appCore);
                auto url = strong_this->appCore.Simulation().Universe().InfoURLForSelection(strong_this->selection);
                strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [strong_this, name, detail, timeLinks, url]()
                    {
                        strong_this->NameLabel().Text(name);
                        strong_this->DetailLabel().Inlines().Clear();
                        if (!detail.empty())
                        {
                            Microsoft::UI::Xaml::Documents::Run detailRun;
                            detailRun.Text(detail);
                            strong_this->DetailLabel().Inlines().Append(detailRun);
                        }
                        for (auto const& link : timeLinks)
                        {
                            if (strong_this->DetailLabel().Inlines().Size() > 0)
                            {
                                Microsoft::UI::Xaml::Documents::Run newline;
                                newline.Text(L"\n");
                                strong_this->DetailLabel().Inlines().Append(newline);
                            }
                            auto label = std::wstring_view(link.Label);
                            auto timeStr = std::wstring_view(link.TimeString);
                            auto pos = label.find(timeStr);
                            if (pos != std::wstring_view::npos)
                            {
                                Microsoft::UI::Xaml::Documents::Run prefixRun;
                                prefixRun.Text(hstring(label.substr(0, pos)));
                                strong_this->DetailLabel().Inlines().Append(prefixRun);

                                Microsoft::UI::Xaml::Documents::Hyperlink hyperlink;
                                auto linkURL = link.URL;
                                hyperlink.Click([weak_this{ strong_this->get_weak() }, linkURL](Microsoft::UI::Xaml::Documents::Hyperlink const&, Microsoft::UI::Xaml::Documents::HyperlinkClickEventArgs const&)
                                    {
                                        auto strong_this{ weak_this.get() };
                                        if (strong_this == nullptr) return;
                                        strong_this->openURLEvent(*strong_this, linkURL);
                                    });
                                Microsoft::UI::Xaml::Documents::Run linkRun;
                                linkRun.Text(hstring(timeStr));
                                hyperlink.Inlines().Append(linkRun);
                                strong_this->DetailLabel().Inlines().Append(hyperlink);

                                auto suffix = label.substr(pos + timeStr.size());
                                if (!suffix.empty())
                                {
                                    Microsoft::UI::Xaml::Documents::Run suffixRun;
                                    suffixRun.Text(hstring(suffix));
                                    strong_this->DetailLabel().Inlines().Append(suffixRun);
                                }
                            }
                            else
                            {
                                Microsoft::UI::Xaml::Documents::Run fallbackRun;
                                fallbackRun.Text(link.Label);
                                strong_this->DetailLabel().Inlines().Append(fallbackRun);
                            }
                        }
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

    event_token InfoUserControl::GetInfo(Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs> const& handler)
    {
        return getInfoEvent.add(handler);
    }

    void InfoUserControl::GetInfo(event_token const& token) noexcept
    {
        getInfoEvent.remove(token);
    }

    event_token InfoUserControl::OpenURL(Windows::Foundation::EventHandler<hstring> const& handler)
    {
        return openURLEvent.add(handler);
    }

    void InfoUserControl::OpenURL(event_token const& token) noexcept
    {
        openURLEvent.remove(token);
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

    void InfoUserControl::ControlStrip_GetInfo(IInspectable const&, CelestiaWinUI::InfoGetInfoArgs const& args)
    {
        getInfoEvent(*this, args);
    }
}