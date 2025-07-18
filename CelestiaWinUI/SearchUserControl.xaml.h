#pragma once

#include "SearchUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct SearchUserControl : SearchUserControlT<SearchUserControl>
    {
        SearchUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        void InitializeComponent();

        void SearchBox_TextChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const&);
        void SearchBox_SuggestionChosen(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs const&);
        void SearchBox_QuerySubmitted(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs const&);
        void ShowSearchResult(CelestiaComponent::CelestiaSelection const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaComponent::CelestiaSelection object;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct SearchUserControl : SearchUserControlT<SearchUserControl, implementation::SearchUserControl>
    {
    };
}
