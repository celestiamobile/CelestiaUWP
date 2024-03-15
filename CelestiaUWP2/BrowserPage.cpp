//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "BrowserPage.h"
#if __has_include("BrowserParameter.g.cpp")
#include "BrowserParameter.g.cpp"
#endif
#if __has_include("BrowserPage.g.cpp")
#include "BrowserPage.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::CelestiaUWP2::implementation
{
    BrowserParameter::BrowserParameter(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer) {}
    CelestiaComponent::CelestiaAppCore BrowserParameter::AppCore() { return appCore; }
    CelestiaComponent::CelestiaRenderer BrowserParameter::Renderer() { return renderer; }

    Collections::IObservableVector<BrowserItem> savedSol = nullptr;
    Collections::IObservableVector<BrowserItem> savedDsos = nullptr;
    BrowserItem savedBrightestStars = nullptr;

    BrowserPage::BrowserPage() : appCore(nullptr), renderer(nullptr)
    {
        rootItems = single_threaded_observable_vector<BrowserItemTab>();
        actions = single_threaded_observable_vector<BrowserAction>
        ({
            BrowserGetInfoAction(),
            BrowserInputAction(LocalizationHelper::Localize(L"Go", L"Go to an object"), 103),
            BrowserInputAction(LocalizationHelper::Localize(L"Follow", L""), 102),
            BrowserInputAction(LocalizationHelper::Localize(L"Sync Orbit", L""), 121),
            BrowserInputAction(LocalizationHelper::Localize(L"Lock Phase", L""), 58),
            BrowserInputAction(LocalizationHelper::Localize(L"Chase", L""), 34),
            BrowserInputAction(LocalizationHelper::Localize(L"Track", L"Track an object"), 116)
        });
    }

    void BrowserPage::NavigationView_SelectionChanged(IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args)
    {
        auto item = args.SelectedItem();
        if (item == nullptr) return;
        auto browserItem = item.try_as<BrowserItemTab>();
        if (browserItem == nullptr) return;
        rootItem = BrowserItem::ConvertToBindable(browserItem.Children());
        propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"RootItem"));
    }

    void BrowserPage::Nav_BackRequested(IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewBackRequestedEventArgs const&)
    {
        Container().Content(BrowserItemListContainer());
        Nav().IsBackEnabled(false);
    }

    void BrowserPage::ActionButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto selectedItem = Tree().SelectedItem();
        if (selectedItem == nullptr) return;
        auto browserItem = selectedItem.try_as<BrowserItem>();
        if (browserItem == nullptr) return;
        auto object = browserItem.Item().Object();
        if (object == nullptr) return;
        auto selection = CelestiaSelection(object);
        auto context = sender.as<Button>().DataContext().as<BrowserAction>();

        if (context.try_as<BrowserGetInfoAction>() != nullptr)
        {
            Container().Navigate(xaml_typename<CelestiaUWP2::InfoPage>(), CelestiaUWP2::InfoParameter(appCore, renderer, selection));
            Nav().IsBackEnabled(true);
        }
        else if (auto inputAction = context.try_as<BrowserInputAction>(); inputAction != nullptr)
        {
            renderer.EnqueueTask([this, selection, inputAction]()
                {
                    appCore.Simulation().Selection(selection);
                    appCore.CharEnter(inputAction.Code());
                });
        }
    }

    Collections::IObservableVector<BrowserItemTab> BrowserPage::RootItems()
    {
        return rootItems;
    }

    Windows::UI::Xaml::Interop::IBindableObservableVector BrowserPage::RootItem()
    {
        return rootItem;
    }

    Collections::IObservableVector<BrowserAction> BrowserPage::Actions()
    {
        return actions;
    }

    event_token BrowserPage::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChangedEvent.add(handler);
    }

    void BrowserPage::PropertyChanged(event_token const& token) noexcept
    {
        propertyChangedEvent.remove(token);
    }

    void BrowserPage::LoadData()
    {
        auto simulation = appCore.Simulation();
        auto universe = simulation.Universe();
        auto observer = simulation.ActiveObserver();
        hstring solarSystemTabName = LocalizationHelper::Localize(L"Solar System", L"Tab for solar system in Star Browser");
        if (savedSol != nullptr)
        {
            rootItems.Append(BrowserItemTab(savedSol, solarSystemTabName));
        }
        else
        {
            auto selection = appCore.Simulation().Find(L"Sol");
            if (!selection.IsEmpty())
            {
                auto star = selection.Object().try_as<CelestiaStar>();
                if (star)
                {
                    CelestiaBrowserItem sol = { universe.StarCatalog().StarName(star), star, [weak_this{ get_weak() }](CelestiaBrowserItem const& item)
                        {
                            auto strong_this{ weak_this.get() };
                            if (strong_this == nullptr) return com_array<CelestiaBrowserItem>();
                            return CelestiaExtension::GetChildren(item, strong_this->appCore);
                        }, false };
                    savedSol = single_threaded_observable_vector<BrowserItem>();
                    savedSol.Append(BrowserItem(sol));
                    rootItems.Append(BrowserItemTab(savedSol, solarSystemTabName));
                }
            }
        }

        auto starRoot = single_threaded_observable_vector<BrowserItem>();
        if (savedBrightestStars != nullptr)
        {
            starRoot.Append(savedBrightestStars);
        }
        else
        {
            auto bsb = universe.StarBrowser(CelestiaStarBrowserType::Brightest, observer);
            auto brightest = bsb.Stars();
            std::vector<CelestiaComponent::CelestiaBrowserItem> stars;
            for (const auto& star : brightest)
            {
                stars.emplace_back(universe.StarCatalog().StarName(star), star, [weak_this{ get_weak() }](CelestiaBrowserItem const& item)
                    {
                        auto strong_this{ weak_this.get() };
                        if (strong_this == nullptr) return com_array<CelestiaBrowserItem>();
                        return CelestiaExtension::GetChildren(item, strong_this->appCore);
                    }, false);
            }
            savedBrightestStars = BrowserItem(CelestiaBrowserItem(LocalizationHelper::Localize(L"Brightest Stars (Absolute Magnitude)", L""), stars, true));
            starRoot.Append(savedBrightestStars);
        }

        // Non static items
        std::vector<std::pair<hstring, CelestiaStarBrowserType>> nonStaticStars
        {
            { LocalizationHelper::Localize(L"Nearest Stars", L""), CelestiaStarBrowserType::Nearest },
            { LocalizationHelper::Localize(L"Brightest Stars", L""), CelestiaStarBrowserType::Brighter },
            { LocalizationHelper::Localize(L"Stars with Planets", L""), CelestiaStarBrowserType::WithPlants },
        };
        for (const auto& [name, type] : nonStaticStars)
        {
            auto sb = universe.StarBrowser(type, observer);
            auto results = sb.Stars();
            std::vector<CelestiaComponent::CelestiaBrowserItem> stars;
            for (const auto& star : results)
            {
                stars.emplace_back(universe.StarCatalog().StarName(star), star, [weak_this{ get_weak() }](CelestiaBrowserItem const& item)
                    {
                        auto strong_this{ weak_this.get() };
                        if (strong_this == nullptr) return com_array<CelestiaBrowserItem>();
                        return CelestiaExtension::GetChildren(item, strong_this->appCore);
                    }, false);
            }
            starRoot.Append(BrowserItem(CelestiaBrowserItem(name, stars, true)));
        }

        rootItems.Append(BrowserItemTab(starRoot, LocalizationHelper::Localize(L"Stars", L"Tab for stars in Star Browser")));

        hstring dsoTabName = LocalizationHelper::Localize(L"DSOs", L"Tab for deep sky objects in Star Browser");
        if (savedDsos != nullptr)
        {
            rootItems.Append(BrowserItemTab(savedDsos, dsoTabName));
        }
        else
        {
            std::vector<std::wstring> typeMap
            {
                L"SB",
                L"S",
                L"E",
                L"Irr",
                L"Neb",
                L"Glob",
                L"Open cluster",
                L"Unknown"
            };
            std::vector<std::vector<CelestiaBrowserItem>> results;
            for (int i = 0; i < typeMap.size(); i++)
            {
                results.emplace_back();
            }
            std::vector<hstring> categoryNames
            {
                LocalizationHelper::Localize(L"Galaxies (Barred Spiral)", L""),
                LocalizationHelper::Localize(L"Galaxies (Spiral)", L""),
                LocalizationHelper::Localize(L"Galaxies (Elliptical)", L""),
                LocalizationHelper::Localize(L"Galaxies (Irregular)", L""),
                LocalizationHelper::Localize(L"Nebulae", L""),
                LocalizationHelper::Localize(L"Globulars", L""),
                LocalizationHelper::Localize(L"Open Clusters", L""),
                LocalizationHelper::Localize(L"Unknown", L""),
            };
            auto dsoCatalog = universe.DSOCatalog();
            for (int i = 0; i < dsoCatalog.Count(); i++)
            {
                auto dso = dsoCatalog.DSOAt(i);
                auto categoryIndex = typeMap.size() - 1;
                auto type = std::wstring(dso.Type());
                for (int j = 0; j < typeMap.size(); j++)
                {
                    if ((typeMap[j].size() <= type.size()) && std::equal(typeMap[j].begin(), typeMap[j].end(), type.begin()))
                    {
                        categoryIndex = j;
                        break;
                    }
                }
                CelestiaBrowserItem item{ dsoCatalog.DSOName(dso), dso, [weak_this{ get_weak() }](CelestiaBrowserItem const& item)
                    {
                        auto strong_this{ weak_this.get() };
                        if (strong_this == nullptr) return com_array<CelestiaBrowserItem>();
                        return CelestiaExtension::GetChildren(item, strong_this->appCore);
                    }, false };
                results[categoryIndex].push_back(item);
            }
            auto dsoCategories = single_threaded_observable_vector<BrowserItem>();
            for (size_t i = 0; i < results.size(); i++)
            {
                if (results[i].size() > 0)
                {
                    dsoCategories.Append(BrowserItem(CelestiaBrowserItem(categoryNames[i], results[i], false)));
                }
            }
            savedDsos = dsoCategories;
            rootItems.Append(BrowserItemTab(savedDsos, dsoTabName));
        }
        Nav().SelectedItem(rootItems.GetAt(0));
    }

    void BrowserPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto parameter = e.Parameter().as<CelestiaUWP2::BrowserParameter>();
        appCore = parameter.AppCore();
        renderer = parameter.Renderer();
        LoadData();
    }
}
