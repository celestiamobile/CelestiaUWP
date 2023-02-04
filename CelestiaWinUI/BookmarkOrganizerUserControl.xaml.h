#pragma once

#include "BookmarkOrganizerUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct BookmarkOrganizerUserControl : BookmarkOrganizerUserControlT<BookmarkOrganizerUserControl>
    {
        BookmarkOrganizerUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, bool canAddNewBookmarks);
        void InitializeComponent();

        Microsoft::UI::Xaml::Interop::IBindableObservableVector Bookmarks();

        Windows::Foundation::IAsyncAction WriteBookmarks();
        void InsertBookmarkAtSelection(CelestiaAppComponent::BookmarkNode const& bookmark);

        fire_and_forget SaveTimer_Tick(Windows::Foundation::IInspectable const&, Windows::Foundation::IInspectable const&);
        fire_and_forget BookmarkOrganizerPage_Unloaded(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

        void DeleteButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget NewFolderButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget RenameButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

        void GoButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> bookmarks;
        Microsoft::UI::Xaml::Interop::IBindableObservableVector bindableBookmarks;
        Microsoft::UI::Xaml::DispatcherTimer saveTimer{ nullptr };
        event_token saveTimerToken;
        bool isRead{ false };
        bool isSaving{ false };
        bool canAddNewBookmarks{ false };

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
