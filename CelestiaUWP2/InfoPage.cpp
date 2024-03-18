//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "InfoPage.h"
#if __has_include("InfoParameter.g.cpp")
#include "InfoParameter.g.cpp"
#endif
#if __has_include("InfoPage.g.cpp")
#include "InfoPage.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::CelestiaUWP2::implementation
{
    InfoParameter::InfoParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaComponent::CelestiaSelection const& selection) : appCore(appCore), renderer(renderer), selection(selection)
    {
    }

    CelestiaComponent::CelestiaAppCore InfoParameter::AppCore()
    {
        return appCore;
    }

    CelestiaComponent::CelestiaRenderer InfoParameter::Renderer()
    {
        return renderer;
    }

    CelestiaComponent::CelestiaSelection InfoParameter::Selection()
    {
        return selection;
    }

    InfoPage::InfoPage() : appCore(nullptr), renderer(nullptr), selection(nullptr)
    {
        actions = single_threaded_observable_vector<BrowserInputAction>
        ({
            BrowserInputAction(LocalizationHelper::Localize(L"Go", L"Go to an object"), 103),
            BrowserInputAction(LocalizationHelper::Localize(L"Follow", L""), 102),
            BrowserInputAction(LocalizationHelper::Localize(L"Sync Orbit", L""), 121),
            BrowserInputAction(LocalizationHelper::Localize(L"Lock Phase", L""), 58),
            BrowserInputAction(LocalizationHelper::Localize(L"Chase", L""), 34),
            BrowserInputAction(LocalizationHelper::Localize(L"Track", L"Track an object"), 116)
        });
    }

    void InfoPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto param = e.Parameter().as<CelestiaUWP2::InfoParameter>();
        appCore = param.AppCore();
        renderer = param.Renderer();
        selection = param.Selection();
        auto obj = selection.Object();
        if (obj == nullptr) return;

        renderer.EnqueueTask([weak_this{ get_weak() }, obj]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                auto name = strong_this->appCore.Simulation().Universe().NameForSelection(strong_this->selection);
                auto detail = SelectionHelper::GetOverview(strong_this->selection, strong_this->appCore);
                auto url = strong_this->selection.InfoURL();
                strong_this->Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [strong_this, name, detail, url]()
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

    Collections::IObservableVector<BrowserInputAction> InfoPage::Actions()
    {
        return actions;
    }

    void InfoPage::ActionButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto action = sender.as<Button>().DataContext().as<BrowserInputAction>();
        renderer.EnqueueTask([weak_this{ get_weak() }, action]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->appCore.Simulation().Selection(strong_this->selection);
                strong_this->appCore.CharEnter(action.Code());
            });
    }
}
