// SearchUserControl.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

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
