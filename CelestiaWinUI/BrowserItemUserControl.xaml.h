
#pragma once

#include "BrowserItemGetInfoArgs.g.h"
#include "BrowserItemUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct BrowserItemGetInfoArgs : BrowserItemGetInfoArgsT<BrowserItemGetInfoArgs>
    {
        BrowserItemGetInfoArgs(CelestiaComponent::CelestiaSelection const& selection) : selection(selection) {}
        CelestiaComponent::CelestiaSelection Selection() { return selection; }
    private:
        CelestiaComponent::CelestiaSelection selection;
    };

    struct BrowserItemUserControl : BrowserItemUserControlT<BrowserItemUserControl>
    {
        BrowserItemUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, CelestiaAppComponent::BrowserItemTab const& item);

        Microsoft::UI::Xaml::Interop::IBindableObservableVector RootItem();
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserAction> Actions();

        void ActionButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

        event_token GetInfo(Windows::Foundation::EventHandler<CelestiaWinUI::BrowserItemGetInfoArgs> const& handler);
        void GetInfo(event_token const& token) noexcept;

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Microsoft::UI::Xaml::Interop::IBindableObservableVector rootItem{ nullptr };
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserAction> actions;
        event<Windows::Foundation::EventHandler<CelestiaWinUI::BrowserItemGetInfoArgs>> getInfoEvent;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct BrowserItemUserControl : BrowserItemUserControlT<BrowserItemUserControl, implementation::BrowserItemUserControl>
    {
    };
}
