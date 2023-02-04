#include "pch.h"
#include "BrowserUserControl.xaml.h"
#if __has_include("BrowserUserControl.g.cpp")
#include "BrowserUserControl.g.cpp"
#endif
#if __has_include("BrowserItem.g.cpp")
#include "BrowserItem.g.cpp"
#endif
#if __has_include("BrowserItemTab.g.cpp")
#include "BrowserItemTab.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::CelestiaWinUI::implementation
{
    Collections::IObservableVector<CelestiaWinUI::BrowserItem> savedSol = nullptr;
    Collections::IObservableVector<CelestiaWinUI::BrowserItem> savedDsos = nullptr;
    CelestiaWinUI::BrowserItem savedBrightestStars = nullptr;
    CelestiaWinUI::BrowserItem savedStarsWithPlanets = nullptr;

    BrowserItem::BrowserItem(CelestiaComponent::CelestiaBrowserItem const& item) : item(item)
    {
        children = single_threaded_observable_vector<CelestiaWinUI::BrowserItem>();
    }

    CelestiaComponent::CelestiaBrowserItem BrowserItem::Item() { return item; }


    hstring BrowserItem::ItemName()
    {
        return item.Name();
    }

    Collections::IObservableVector<CelestiaWinUI::BrowserItem> BrowserItem::Children()
    {
        if (children.Size() == 0)
        {
            for (const auto& child : item.Children())
            {
                children.Append(CelestiaWinUI::BrowserItem(child));
            }
        }
        return children;
    }

    BrowserItemTab::BrowserItemTab(Collections::IObservableVector<CelestiaWinUI::BrowserItem> const& children, hstring const& tabName) : tabName(tabName), children(children) {}

    hstring BrowserItemTab::TabName()
    {
        return tabName;
    }

    Collections::IObservableVector<CelestiaWinUI::BrowserItem> BrowserItemTab::Children()
    {
        return children;
    }

    BrowserUserControl::BrowserUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer)
    {
        rootItems = single_threaded_observable_vector<CelestiaWinUI::BrowserItemTab>();
        InitializeComponent();

        LoadData();
        SetUpActions();
    }

    void BrowserUserControl::NavigationView_SelectionChanged(IInspectable const&, NavigationViewSelectionChangedEventArgs const& args)
    {
        auto item = args.SelectedItem();
        if (item == nullptr) return;
        auto browserItem = item.try_as<CelestiaWinUI::BrowserItemTab>();
        if (browserItem == nullptr) return;
        rootItem = browserItem.Children();
        propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"RootItem"));
    }

    Collections::IObservableVector<CelestiaWinUI::BrowserItemTab> BrowserUserControl::RootItems()
    {
        return rootItems;
    }

    Collections::IObservableVector<CelestiaWinUI::BrowserItem> BrowserUserControl::RootItem()
    {
        return rootItem;
    }

    event_token BrowserUserControl::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChangedEvent.add(handler);
    }

    void BrowserUserControl::PropertyChanged(event_token const& token) noexcept
    {
        propertyChangedEvent.remove(token);
    }

    void BrowserUserControl::SetUpActions()
    {
        std::vector<std::pair<hstring, int16_t>> actions
        {
            { L"Go", (int16_t)103 },
            { L"Follow", (int16_t)102 },
            { L"Sync Orbit", (int16_t)121 },
            { L"Lock Phase", (int16_t)58 },
            { L"Chase", (int16_t)34 },
            { L"Track", (int16_t)116 }
        };
        for (const auto& [name, code] : actions)
        {
            auto copiedCode = code;
            Button button;
            button.Content(box_value(LocalizationHelper::Localize(name)));
            button.Click([this, copiedCode](IInspectable const&, RoutedEventArgs const&)
                {
                    auto selectedItem = Tree().SelectedItem();
                    if (selectedItem == nullptr) return;
                    auto browserItem = selectedItem.try_as<CelestiaWinUI::BrowserItem>();
                    if (browserItem == nullptr) return;
                    renderer.EnqueueTask([this, browserItem, copiedCode]()
                        {
                            appCore.Simulation().Selection(CelestiaSelection(browserItem.Item().Object()));
                            appCore.CharEnter(copiedCode);
                        });
                });
            ButtonStack().Children().Append(button);
        }
    }

    void BrowserUserControl::LoadData()
    {
        hstring solarSystemTabName = LocalizationHelper::Localize(L"Solar System");
        if (savedSol != nullptr)
        {
            rootItems.Append(CelestiaWinUI::BrowserItemTab(savedSol, solarSystemTabName));
        }
        else
        {
            auto selection = appCore.Simulation().Find(L"Sol");
            if (!selection.IsEmpty())
            {
                auto star = selection.Object().try_as<CelestiaStar>();
                if (star)
                {
                    CelestiaBrowserItem sol = { appCore.Simulation().Universe().StarCatalog().StarName(star), star, [this](CelestiaBrowserItem const& item) { return CelestiaExtension::GetChildren(item, appCore); }, false };
                    savedSol = single_threaded_observable_vector<CelestiaWinUI::BrowserItem>();
                    savedSol.Append(CelestiaWinUI::BrowserItem(sol));
                    rootItems.Append(CelestiaWinUI::BrowserItemTab(savedSol, solarSystemTabName));
                }
            }
        }

        auto starRoot = single_threaded_observable_vector<CelestiaWinUI::BrowserItem>();
        if (savedBrightestStars != nullptr)
        {
            starRoot.Append(savedBrightestStars);
        }
        else
        {
            auto bsb = appCore.Simulation().StarBrowser(CelestiaStarBrowserType::Brightest);
            auto brightest = bsb.Stars();
            std::vector<CelestiaComponent::CelestiaBrowserItem> stars;
            for (const auto& star : brightest)
            {
                stars.emplace_back(appCore.Simulation().Universe().StarCatalog().StarName(star), star, [this](CelestiaBrowserItem const& item) { return CelestiaExtension::GetChildren(item, appCore); }, false);
            }
            savedBrightestStars = CelestiaWinUI::BrowserItem(CelestiaBrowserItem(LocalizationHelper::Localize(L"Brightest Stars (Absolute Magnitude)"), stars, true));
            starRoot.Append(savedBrightestStars);
        }

        if (savedStarsWithPlanets != nullptr)
        {
            starRoot.Append(savedStarsWithPlanets);
        }
        else
        {
            auto hsb = appCore.Simulation().StarBrowser(CelestiaStarBrowserType::WithPlants);
            auto hasPlanets = hsb.Stars();
            std::vector<CelestiaComponent::CelestiaBrowserItem> stars;
            for (const auto& star : hasPlanets)
            {
                stars.emplace_back(appCore.Simulation().Universe().StarCatalog().StarName(star), star, [this](CelestiaBrowserItem const& item) { return CelestiaExtension::GetChildren(item, appCore); }, false);
            }
            savedStarsWithPlanets = CelestiaWinUI::BrowserItem(CelestiaBrowserItem(LocalizationHelper::Localize(LocalizationHelper::Localize(L"Stars with Planets")), stars, true));
            starRoot.Append(savedBrightestStars);
        }

        // Non static items
        std::vector<std::pair<hstring, CelestiaStarBrowserType>> nonStaticStars
        {
            { L"Nearest Stars", CelestiaStarBrowserType::Nearest },
            { L"Brightest Stars", CelestiaStarBrowserType::Brighter },
        };
        for (const auto& [name, type] : nonStaticStars)
        {
            auto sb = appCore.Simulation().StarBrowser(CelestiaStarBrowserType::WithPlants);
            auto results = sb.Stars();
            std::vector<CelestiaComponent::CelestiaBrowserItem> stars;
            for (const auto& star : results)
            {
                stars.emplace_back(appCore.Simulation().Universe().StarCatalog().StarName(star), star, [this](CelestiaBrowserItem const& item) { return CelestiaExtension::GetChildren(item, appCore); }, false);
            }
            starRoot.Append(CelestiaWinUI::BrowserItem(CelestiaBrowserItem(LocalizationHelper::Localize(LocalizationHelper::Localize(name)), stars, true)));
        }

        rootItems.Append(CelestiaWinUI::BrowserItemTab(starRoot, LocalizationHelper::Localize(L"Stars")));

        hstring dsoTabName = LocalizationHelper::Localize(L"DSOs");
        if (savedDsos != nullptr)
        {
            rootItems.Append(CelestiaWinUI::BrowserItemTab(savedDsos, dsoTabName));
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
                LocalizationHelper::Localize(L"Galaxies (Barred Spiral)"),
                LocalizationHelper::Localize(L"Galaxies (Spiral)"),
                LocalizationHelper::Localize(L"Galaxies (Elliptical)"),
                LocalizationHelper::Localize(L"Galaxies (Irregular)"),
                LocalizationHelper::Localize(L"Nebulae"),
                LocalizationHelper::Localize(L"Globulars"),
                LocalizationHelper::Localize(L"Open Clusters"),
                LocalizationHelper::Localize(L"Unknown"),
            };
            auto dsoCatalog = appCore.Simulation().Universe().DSOCatalog();
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
                CelestiaBrowserItem item{ dsoCatalog.DSOName(dso), dso, [this](CelestiaBrowserItem const& item) { return CelestiaExtension::GetChildren(item, appCore); }, false };
                results[categoryIndex].push_back(item);
            }
            auto dsoCategories = single_threaded_observable_vector<CelestiaWinUI::BrowserItem>();
            for (size_t i = 0; i < results.size(); i++)
            {
                if (results[i].size() > 0)
                {
                    dsoCategories.Append(CelestiaWinUI::BrowserItem(CelestiaBrowserItem(categoryNames[i], results[i], false)));
                }
            }
            savedDsos = dsoCategories;
            rootItems.Append(CelestiaWinUI::BrowserItemTab(savedDsos, dsoTabName));
        }
        Nav().SelectedItem(rootItems.GetAt(0));
    }
}
