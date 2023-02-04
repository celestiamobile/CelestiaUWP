#pragma once

#include "BookmarkOrganizerUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct BookmarkOrganizerUserControl : BookmarkOrganizerUserControlT<BookmarkOrganizerUserControl>
    {
        BookmarkOrganizerUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);

        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> Bookmarks();
        void Bookmarks(Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> const&);

        Windows::Foundation::IAsyncAction WriteBookmarks();
        Windows::Foundation::IAsyncAction InsertBookmarkAtSelection(CelestiaAppComponent::BookmarkNode const& bookmark);

        fire_and_forget DeleteButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget NewFolderButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget RenameButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

        void GoButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> bookmarks;

        fire_and_forget ReadBookmarks();
        Windows::Foundation::IAsyncAction CreateNewFolder();
        std::pair<CelestiaAppComponent::BookmarkNode, CelestiaAppComponent::BookmarkNode> GetSelectionInfo();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct BookmarkOrganizerUserControl : BookmarkOrganizerUserControlT<BookmarkOrganizerUserControl, implementation::BookmarkOrganizerUserControl>
    {
    };
}
