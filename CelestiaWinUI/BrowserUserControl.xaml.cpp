#include "pch.h"
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

        userControl.GetInfo({ get_weak(), &BrowserUserControl::BrowserItem_GetInfo });

        BrowserItemListContainer().Children().Append(userControl);
        BrowserItemListContainer().SetAlignTopWithPanel(userControl, true);
        BrowserItemListContainer().SetAlignBottomWithPanel(userControl, true);
        BrowserItemListContainer().SetAlignLeftWithPanel(userControl, true);
        BrowserItemListContainer().SetAlignRightWithPanel(userControl, true);
    }

    void BrowserUserControl::Nav_BackRequested(IInspectable const&, Controls::NavigationViewBackRequestedEventArgs const&)
    {
        Container().Content(BrowserItemListContainer());
        Nav().IsBackEnabled(false);
    }

    void BrowserUserControl::BrowserItem_GetInfo(IInspectable const&, CelestiaWinUI::InfoGetInfoArgs const& args)
    {
        args.Handled(true);
        InfoUserControl userControl{ appCore, renderer, args.Selection() };
        Container().Navigate(xaml_typename<CelestiaWinUI::CustomPage>(), userControl);
        Nav().IsBackEnabled(true);
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
}
