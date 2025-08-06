// BrowserUserControl.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"

#include <unordered_map>

#include "BrowserUserControl.xaml.h"
#if __has_include("BrowserUserControl.g.cpp")
#include "BrowserUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::CelestiaWinUI::implementation
{
    Collections::IObservableVector<BrowserItem> savedSol = nullptr;
    Collections::IObservableVector<BrowserItem> savedDsos = nullptr;
    BrowserItem savedBrightestStars = nullptr;

    BrowserUserControl::BrowserUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer)
    {
        rootItems = single_threaded_observable_vector<BrowserItemTab>();
    }

    void BrowserUserControl::InitializeComponent()
    {
        BrowserUserControlT::InitializeComponent();

        LoadData();
    }

    void BrowserUserControl::NavigationView_SelectionChanged(IInspectable const&, NavigationViewSelectionChangedEventArgs const& args)
    {
        BrowserItemListContainer().Children().Clear();

        auto item = args.SelectedItem();
        if (item == nullptr) return;
        auto browserItem = item.try_as<BrowserItemTab>();
        if (browserItem == nullptr) return;
        BrowserItemUserControl userControl{ appCore, renderer, browserItem, false };

        BrowserItemListContainer().Children().Append(userControl);
        BrowserItemListContainer().SetAlignTopWithPanel(userControl, true);
        BrowserItemListContainer().SetAlignBottomWithPanel(userControl, true);
        BrowserItemListContainer().SetAlignLeftWithPanel(userControl, true);
        BrowserItemListContainer().SetAlignRightWithPanel(userControl, true);
    }

    Collections::IObservableVector<BrowserItemTab> BrowserUserControl::RootItems()
    {
        return rootItems;
    }

    void BrowserUserControl::LoadData()
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
            auto dsoCatalog = universe.DSOCatalog();
            std::pair<hstring, std::vector<CelestiaBrowserItem>> barredSpiralItems = { LocalizationHelper::Localize(L"Barred Spiral Galaxies", L""), {} };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> spiralItems = { LocalizationHelper::Localize(L"Spiral Galaxies", L""), {} };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> ellipticalItems = { LocalizationHelper::Localize(L"Elliptical Galaxies", L""), {} };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> irregularItems = { LocalizationHelper::Localize(L"Irregular Galaxies", L""), {} };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> lenticularItems = { LocalizationHelper::Localize(L"Lenticular Galaxies", L""), {} };
            std::unordered_map<hstring, std::pair<hstring, std::vector<CelestiaBrowserItem>>&> galaxyItems = {
                { L"SBa", barredSpiralItems },
                { L"SBb", barredSpiralItems },
                { L"SBc", barredSpiralItems },
                { L"Sa", spiralItems },
                { L"Sb", spiralItems },
                { L"Sc", spiralItems },
                { L"S0", lenticularItems },
                { L"E0", ellipticalItems },
                { L"E1", ellipticalItems },
                { L"E2", ellipticalItems },
                { L"E3", ellipticalItems },
                { L"E4", ellipticalItems },
                { L"E5", ellipticalItems },
                { L"E6", ellipticalItems },
                { L"E7", ellipticalItems },
                { L"Irr", irregularItems },
            };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> emissionItems = { LocalizationHelper::Localize(L"Emission Nebulae", L""), {} };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> reflectionItems = { LocalizationHelper::Localize(L"Reflection Nebulae", L""), {} };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> darkItems = { LocalizationHelper::Localize(L"Dark Nebulae", L""), {} };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> planetaryItems = { LocalizationHelper::Localize(L"Planetary Nebulae", L""), {} };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> supernovaRemnantItems = { LocalizationHelper::Localize(L"Supernova Remnants", L""), {} };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> hiiRegionItems = { LocalizationHelper::Localize(L"H II Regions", L""), {} };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> protoplanetaryItems = { LocalizationHelper::Localize(L"Protoplanetary Nebulae", L""), {} };
            std::pair<hstring, std::vector<CelestiaBrowserItem>> unknownItems = { LocalizationHelper::Localize(L"Unknown Nebulae", L""), {} };
            std::unordered_map<hstring, std::pair<hstring, std::vector<CelestiaBrowserItem>>&> nebulaItems = {
                { L"Emission", emissionItems },
                { L"Reflection", reflectionItems },
                { L"Dark", darkItems },
                { L"Planetary", planetaryItems },
                { L"SupernovaRemnant", supernovaRemnantItems },
                { L"HII_Region", hiiRegionItems },
                { L"Protoplanetary", protoplanetaryItems },
                { L" ", unknownItems },
            };
            std::vector<CelestiaBrowserItem> globularItems;
            std::vector<CelestiaBrowserItem> openClusterItems;

            for (int i = 0; i < dsoCatalog.Count(); i++)
            {
                auto dso = dsoCatalog.DSOAt(i);
                std::vector<CelestiaBrowserItem>* vectorToAdd = nullptr;
                hstring type = dso.Type();
                switch (dso.ObjectType())
                {
                case CelestiaDSOType::Galaxy:
                    if (auto it = galaxyItems.find(type); it != galaxyItems.end())
                        vectorToAdd = &it->second.second;
                    break;
                case CelestiaDSOType::Globular:
                    vectorToAdd = &globularItems;
                    break;
                case CelestiaDSOType::Nebula:
                    if (auto it = nebulaItems.find(type); it != nebulaItems.end())
                        vectorToAdd = &it->second.second;
                    break;
                case CelestiaDSOType::OpenCluster:
                    vectorToAdd = &openClusterItems;
                    break;
                }

                if (vectorToAdd == nullptr) continue;

                CelestiaBrowserItem item{ dsoCatalog.DSOName(dso), dso, [weak_this{ get_weak() }](CelestiaBrowserItem const& item)
                {
                    auto strong_this{ weak_this.get() };
                    if (strong_this == nullptr) return com_array<CelestiaBrowserItem>();
                    return CelestiaExtension::GetChildren(item, strong_this->appCore);
                }, false };
                vectorToAdd->push_back(item);
            }
            auto dsoCategories = single_threaded_observable_vector<BrowserItem>();
            std::vector<CelestiaBrowserItem> galaxyBrowserItems;
            for (const auto& [name, items] : { barredSpiralItems, spiralItems, ellipticalItems, lenticularItems, irregularItems })
            {
                if (items.empty())
                    continue;
                galaxyBrowserItems.push_back(CelestiaBrowserItem(name, items, false));
            }
            dsoCategories.Append(BrowserItem(CelestiaBrowserItem(LocalizationHelper::Localize(L"Galaxies", L""), galaxyBrowserItems, true)));
            dsoCategories.Append(BrowserItem(CelestiaBrowserItem(LocalizationHelper::Localize(L"Globulars", L""), globularItems, false)));
            std::vector<CelestiaBrowserItem> nebulaBrowserItems;
            for (const auto& [name, items] : { emissionItems, reflectionItems, darkItems, planetaryItems, supernovaRemnantItems, hiiRegionItems, protoplanetaryItems, unknownItems })
            {
                if (items.empty())
                    continue;
                nebulaBrowserItems.push_back(CelestiaBrowserItem(name, items, false));
            }
            dsoCategories.Append(BrowserItem(CelestiaBrowserItem(LocalizationHelper::Localize(L"Nebulae", L""), nebulaBrowserItems, true)));
            dsoCategories.Append(BrowserItem(CelestiaBrowserItem(LocalizationHelper::Localize(L"Open Clusters", L""), openClusterItems, false)));
            savedDsos = dsoCategories;
            rootItems.Append(BrowserItemTab(savedDsos, dsoTabName));
        }
        Nav().SelectedItem(rootItems.GetAt(0));
    }
}
