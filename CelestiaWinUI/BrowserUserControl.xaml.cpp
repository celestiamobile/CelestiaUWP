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
        InitializeComponent();

        LoadData();
        SetUpActions();
    }

    void BrowserUserControl::NavigationView_SelectionChanged(IInspectable const&, NavigationViewSelectionChangedEventArgs const& args)
    {
        auto item = args.SelectedItem();
        if (item == nullptr) return;
        auto browserItem = item.try_as<BrowserItemTab>();
        if (browserItem == nullptr) return;
        rootItem = BrowserItem::ConvertToBindable(browserItem.Children());
        propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"RootItem"));
    }

    void BrowserUserControl::Nav_BackRequested(IInspectable const&, Controls::NavigationViewBackRequestedEventArgs const&)
    {
        Container().Content(BrowserItemListContainer());
        Nav().IsBackEnabled(false);
    }

    void BrowserUserControl::GetInfoButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto selectedItem = Tree().SelectedItem();
        if (selectedItem == nullptr) return;
        auto browserItem = selectedItem.try_as<BrowserItem>();
        if (browserItem == nullptr) return;
        auto object = browserItem.Item().Object();
        if (object == nullptr) return;
        auto selection = CelestiaSelection(object);
        InfoUserControl userControl{ appCore, selection };
        Container().Navigate(xaml_typename<CelestiaWinUI::CustomPage>(), userControl);
        Nav().IsBackEnabled(true);
    }

    Collections::IObservableVector<BrowserItemTab> BrowserUserControl::RootItems()
    {
        return rootItems;
    }

    Microsoft::UI::Xaml::Interop::IBindableObservableVector BrowserUserControl::RootItem()
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
        Button getInfoButton;
        getInfoButton.Content(box_value(LocalizationHelper::Localize(L"Get Info")));
        getInfoButton.Click({ this, &BrowserUserControl::GetInfoButton_Click });
        ButtonStack().Children().Append(getInfoButton);
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
                    auto browserItem = selectedItem.try_as<BrowserItem>();
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
        auto simulation = appCore.Simulation();
        auto universe = simulation.Universe();
        auto observer = simulation.ActiveObserver();
        hstring solarSystemTabName = LocalizationHelper::Localize(L"Solar System");
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
                    CelestiaBrowserItem sol = { universe.StarCatalog().StarName(star), star, [this](CelestiaBrowserItem const& item) { return CelestiaExtension::GetChildren(item, appCore); }, false };
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
                stars.emplace_back(universe.StarCatalog().StarName(star), star, [this](CelestiaBrowserItem const& item) { return CelestiaExtension::GetChildren(item, appCore); }, false);
            }
            savedBrightestStars = BrowserItem(CelestiaBrowserItem(LocalizationHelper::Localize(L"Brightest Stars (Absolute Magnitude)"), stars, true));
            starRoot.Append(savedBrightestStars);
        }

        // Non static items
        std::vector<std::pair<hstring, CelestiaStarBrowserType>> nonStaticStars
        {
            { L"Nearest Stars", CelestiaStarBrowserType::Nearest },
            { L"Brightest Stars", CelestiaStarBrowserType::Brighter },
            { L"Stars with Planets", CelestiaStarBrowserType::WithPlants },
        };
        for (const auto& [name, type] : nonStaticStars)
        {
            auto sb = universe.StarBrowser(type, observer);
            auto results = sb.Stars();
            std::vector<CelestiaComponent::CelestiaBrowserItem> stars;
            for (const auto& star : results)
            {
                stars.emplace_back(universe.StarCatalog().StarName(star), star, [this](CelestiaBrowserItem const& item) { return CelestiaExtension::GetChildren(item, appCore); }, false);
            }
            starRoot.Append(BrowserItem(CelestiaBrowserItem(LocalizationHelper::Localize(LocalizationHelper::Localize(name)), stars, true)));
        }

        rootItems.Append(BrowserItemTab(starRoot, LocalizationHelper::Localize(L"Stars")));

        hstring dsoTabName = LocalizationHelper::Localize(L"DSOs");
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
                LocalizationHelper::Localize(L"Galaxies (Barred Spiral)"),
                LocalizationHelper::Localize(L"Galaxies (Spiral)"),
                LocalizationHelper::Localize(L"Galaxies (Elliptical)"),
                LocalizationHelper::Localize(L"Galaxies (Irregular)"),
                LocalizationHelper::Localize(L"Nebulae"),
                LocalizationHelper::Localize(L"Globulars"),
                LocalizationHelper::Localize(L"Open Clusters"),
                LocalizationHelper::Localize(L"Unknown"),
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
                CelestiaBrowserItem item{ dsoCatalog.DSOName(dso), dso, [this](CelestiaBrowserItem const& item) { return CelestiaExtension::GetChildren(item, appCore); }, false };
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
