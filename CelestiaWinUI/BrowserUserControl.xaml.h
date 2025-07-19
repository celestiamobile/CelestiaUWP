
#pragma once

#include "BrowserUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct BrowserUserControl : BrowserUserControlT<BrowserUserControl>
    {
        BrowserUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        void InitializeComponent();

        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItemTab> RootItems();

        void NavigationView_SelectionChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserItemTab> rootItems;

        void LoadData();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct BrowserUserControl : BrowserUserControlT<BrowserUserControl, implementation::BrowserUserControl>
    {
    };
}
