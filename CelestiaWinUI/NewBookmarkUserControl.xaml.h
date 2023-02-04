#pragma once

#include "NewBookmarkUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct NewBookmarkUserControl : NewBookmarkUserControlT<NewBookmarkUserControl>
    {
        NewBookmarkUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        void InitializeComponent();

        hstring NameText();
        void NameText(hstring const&);

        void ConfirmButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        hstring nameText{ L"" };
        CelestiaWinUI::BookmarkOrganizerUserControl organizer;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct NewBookmarkUserControl : NewBookmarkUserControlT<NewBookmarkUserControl, implementation::NewBookmarkUserControl>
    {
    };
}
