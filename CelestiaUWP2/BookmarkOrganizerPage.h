//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "BookmarkOrganizerParameter.g.h"
#include "BookmarkOrganizerPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct BookmarkOrganizerParameter : BookmarkOrganizerParameterT<BookmarkOrganizerParameter>
    {
        BookmarkOrganizerParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, bool canAddNewBookmarks);
        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
        bool CanAddNewBookmarks();
    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        bool canAddNewBookmarks;
    };

    struct BookmarkOrganizerPage : BookmarkOrganizerPageT<BookmarkOrganizerPage>
    {
        BookmarkOrganizerPage();
        void InitializeComponent();

        Windows::UI::Xaml::Interop::IBindableObservableVector Bookmarks();

        Windows::Foundation::IAsyncAction WriteBookmarks();
        void InsertBookmarkAtSelection(CelestiaAppComponent::BookmarkNode const& bookmark);

        fire_and_forget SaveTimer_Tick(Windows::Foundation::IInspectable const&, Windows::Foundation::IInspectable const&);
        fire_and_forget BookmarkOrganizerPage_Unloaded(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

        void DeleteButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget NewFolderButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget RenameButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);

        void GoButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> bookmarks;
        Windows::UI::Xaml::Interop::IBindableObservableVector bindableBookmarks;
        Windows::UI::Xaml::DispatcherTimer saveTimer{ nullptr };
        event_token saveTimerToken;
        bool isRead{ false };
        bool isSaving{ false };
        bool canAddNewBookmarks{ false };

        fire_and_forget ReadBookmarks();
        Windows::Foundation::IAsyncAction CreateNewFolder();
        std::pair<CelestiaAppComponent::BookmarkNode, CelestiaAppComponent::BookmarkNode> GetSelectionInfo();
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct BookmarkOrganizerParameter : BookmarkOrganizerParameterT<BookmarkOrganizerParameter, implementation::BookmarkOrganizerParameter>
    {
    };

    struct BookmarkOrganizerPage : BookmarkOrganizerPageT<BookmarkOrganizerPage, implementation::BookmarkOrganizerPage>
    {
    };
}
