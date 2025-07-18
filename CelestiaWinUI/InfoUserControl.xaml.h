#pragma once

#include "InfoUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct InfoUserControl : InfoUserControlT<InfoUserControl>
    {
        InfoUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaComponent::CelestiaSelection const& selection, bool preserveMargin = true);
        void InitializeComponent();

        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserInputAction> Actions();
        void ActionButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaComponent::CelestiaSelection selection;
        bool preserveMargin;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserInputAction> actions;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct InfoUserControl : InfoUserControlT<InfoUserControl, implementation::InfoUserControl>
    {
    };
}
