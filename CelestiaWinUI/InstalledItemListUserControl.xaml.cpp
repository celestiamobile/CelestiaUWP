#include "pch.h"
#include "InstalledItemListUserControl.xaml.h"
#if __has_include("InstalledItemListUserControl.g.cpp")
#include "InstalledItemListUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::CelestiaWinUI::implementation
{
    InstalledItemListUserControl::InstalledItemListUserControl(ResourceManager const& resourceManager) : resourceManager(resourceManager)
    {
        items = single_threaded_observable_vector<CelestiaAppComponent::ResourceItem>();
        InitializeComponent();
        Title().Text(LocalizationHelper::Localize(L"Installed"));
        LoadItems();
    }

    Collections::IObservableVector<CelestiaAppComponent::ResourceItem> InstalledItemListUserControl::Items()
    {
        return items;
    }

    event_token InstalledItemListUserControl::ItemSelected(EventHandler<CelestiaWinUI::InstalledItemListItemSelectedArgs> const& handler)
    {
        return itemSelectedEvent.add(handler);
    }

    void InstalledItemListUserControl::ItemSelected(event_token const& token)
    {
        itemSelectedEvent.remove(token);
    }

    fire_and_forget InstalledItemListUserControl::LoadItems()
    {
        auto installedItems = co_await resourceManager.InstalledItems();
        items.ReplaceAll(std::vector<CelestiaAppComponent::ResourceItem>{ installedItems.begin(), installedItems.end() });
    }

    void InstalledItemListUserControl::ItemList_ItemClick(IInspectable const&, ItemClickEventArgs const& args)
    {
        auto item = args.ClickedItem().try_as<CelestiaAppComponent::ResourceItem>();
        if (item != nullptr)
            itemSelectedEvent(*this, make<InstalledItemListItemSelectedArgs>(item.as<CelestiaAppComponent::ResourceItem>()));
    }
}