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
#include <vector>

namespace winrt::CelestiaWinUI::implementation
{
    struct SearchUserControl : SearchUserControlT<SearchUserControl>
    {
        SearchUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        void InitializeComponent();

        void SearchBox_TextChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const&);
        void SearchBox_SuggestionChosen(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs const&);
        void SearchBox_QuerySubmitted(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs const&);
        void ShowSearchResult(CelestiaComponent::CelestiaSelection const&, hstring const& query = L"");
        void RecentSearchList_ItemClick(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::ItemClickEventArgs const&);
        void ClearHistoryButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

        event_token OpenURL(Windows::Foundation::EventHandler<hstring> const& handler);
        void OpenURL(event_token const& token) noexcept;
        event_token GetInfo(Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs> const& handler);
        void GetInfo(event_token const& token) noexcept;
        event_token ShowSubsystem(Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs> const& handler);
        void ShowSubsystem(event_token const& token) noexcept;

    private:
        std::vector<hstring> ReadRecentSearches();
        void UpdateRecentSearches();
        void RecordRecentSearch(hstring const& path);
        void PresentSearchResult(CelestiaComponent::CelestiaSelection const& selection);
        uint64_t searchGeneration = 0;
        uint64_t selectionGeneration = 0;
        uint64_t recentSearchGeneration = 0;
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        event<Windows::Foundation::EventHandler<hstring>> openURLEvent;
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs>> getInfoEvent;
        event<Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs>> showSubsystemEvent;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct SearchUserControl : SearchUserControlT<SearchUserControl, implementation::SearchUserControl>
    {
    };
}
