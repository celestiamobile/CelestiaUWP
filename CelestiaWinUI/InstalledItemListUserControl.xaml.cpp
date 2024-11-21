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
    InstalledItemListUserControl::InstalledItemListUserControl(ResourceManager const& resourceManager, CelestiaWinUI::GetAddonHandler const& getAddonHandler) : resourceManager(resourceManager), getAddonHandler(getAddonHandler)
    {
        items = single_threaded_observable_vector<CelestiaAppComponent::ResourceItem>();
    }

    void InstalledItemListUserControl::InitializeComponent()
    {
        InstalledItemListUserControlT::InitializeComponent();
        Title().Text(LocalizationHelper::Localize(L"Installed", L"Title for the list of installed add-ons"));
        EmptyHintText().Text(LocalizationHelper::Localize(L"Enhance Celestia with online add-ons", L""));
        EmptyHintButton().Content(box_value(LocalizationHelper::Localize(L"Get Add-ons", L"Open webpage for downloading add-ons")));
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

    void InstalledItemListUserControl::Reload()
    {
        LoadItems();
    }

    fire_and_forget InstalledItemListUserControl::LoadItems()
    {
        auto weak_this{ get_weak() };
        LoadingIndicator().Visibility(Visibility::Visible);
        EmptyHint().Visibility(Visibility::Collapsed);
        ItemList().Visibility(Visibility::Collapsed);
        auto installedItems = co_await resourceManager.InstalledItems();

        auto strong_this = weak_this.get();
        if (strong_this == nullptr)
            co_return;

        strong_this->items.ReplaceAll(std::vector<CelestiaAppComponent::ResourceItem>{ installedItems.begin(), installedItems.end() });
        strong_this->LoadingIndicator().Visibility(Visibility::Collapsed);
        if (strong_this->items.Size() == 0)
            strong_this->EmptyHint().Visibility(Visibility::Visible);
        else
            strong_this->ItemList().Visibility(Visibility::Visible);
    }

    void InstalledItemListUserControl::ItemList_ItemClick(IInspectable const&, ItemClickEventArgs const& args)
    {
        auto item = args.ClickedItem().try_as<CelestiaAppComponent::ResourceItem>();
        if (item != nullptr)
            itemSelectedEvent(*this, make<InstalledItemListItemSelectedArgs>(item.as<CelestiaAppComponent::ResourceItem>()));
    }

    void InstalledItemListUserControl::GetAddonButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        getAddonHandler();
    }
}
