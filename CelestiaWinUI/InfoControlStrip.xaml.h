#pragma once

#include "InfoShowSubsystemArgs.g.h"
#include "InfoGetInfoArgs.g.h"
#include "InfoTemplateSelector.g.h"
#include "InfoControlStrip.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct InfoShowSubsystemArgs : InfoShowSubsystemArgsT<InfoShowSubsystemArgs>
    {
        InfoShowSubsystemArgs(CelestiaComponent::CelestiaSelection const& selection) : selection(selection) {}
        CelestiaComponent::CelestiaSelection Selection() { return selection; }
        bool Handled() { return handled; }
        void Handled(bool value) { handled = value; }
    private:
        CelestiaComponent::CelestiaSelection selection;
        bool handled{ false };
    };

    struct InfoGetInfoArgs : InfoGetInfoArgsT<InfoGetInfoArgs>
    {
        InfoGetInfoArgs(CelestiaComponent::CelestiaSelection const& selection) : selection(selection) {}
        CelestiaComponent::CelestiaSelection Selection() { return selection; }
        bool Handled() { return handled; }
        void Handled(bool value) { handled = value; }
    private:
        CelestiaComponent::CelestiaSelection selection;
        bool handled{ false };
    };

    struct InfoTemplateSelector : InfoTemplateSelectorT<InfoTemplateSelector>
    {
        InfoTemplateSelector();
        Microsoft::UI::Xaml::DataTemplate Action();
        void Action(Microsoft::UI::Xaml::DataTemplate const&);
        Microsoft::UI::Xaml::DataTemplate DropDown();
        void DropDown(Microsoft::UI::Xaml::DataTemplate const&);

        Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(Windows::Foundation::IInspectable const& item, Microsoft::UI::Xaml::DependencyObject const&);
        Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(Windows::Foundation::IInspectable const& item);

    private:
        Microsoft::UI::Xaml::DataTemplate action{ nullptr };
        Microsoft::UI::Xaml::DataTemplate dropDown{ nullptr };
    };

    struct InfoControlStrip : InfoControlStripT<InfoControlStrip>
    {
        InfoControlStrip();

        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
        CelestiaComponent::CelestiaSelection Selection();
        void AppCore(CelestiaComponent::CelestiaAppCore const&);
        void Renderer(CelestiaComponent::CelestiaRenderer const&);
        void Selection(CelestiaComponent::CelestiaSelection const&);

        bool ShowsGetInfo();
        void ShowsGetInfo(bool);

        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserAction> Actions();
        void ActionButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void MarkButton_Loaded(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void MarkMenuFlyoutItem_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

        event_token ShowSubsystem(Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs> const& handler);
        void ShowSubsystem(event_token const& token) noexcept;
        event_token GetInfo(Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs> const& handler);
        void GetInfo(event_token const& token) noexcept;

    private:
        bool showsGetInfo;
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaComponent::CelestiaSelection selection;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BrowserAction> actions;
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs>> showSubsystemEvent;
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs>> getInfoEvent;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct InfoTemplateSelector : InfoTemplateSelectorT<InfoTemplateSelector, implementation::InfoTemplateSelector>
    {
    };

    struct InfoControlStrip : InfoControlStripT<InfoControlStrip, implementation::InfoControlStrip>
    {
    };
}
