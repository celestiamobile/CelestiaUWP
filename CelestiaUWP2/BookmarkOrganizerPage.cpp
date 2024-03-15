//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "BookmarkOrganizerPage.h"
#if __has_include("BookmarkOrganizerParameter.g.cpp")
#include "BookmarkOrganizerParameter.g.cpp"
#endif
#if __has_include("BookmarkOrganizerPage.g.cpp")
#include "BookmarkOrganizerPage.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::CelestiaUWP2::implementation
{
    BookmarkOrganizerParameter::BookmarkOrganizerParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, bool canAddNewBookmarks) : appCore(appCore), renderer(renderer), canAddNewBookmarks(canAddNewBookmarks) {}
    CelestiaComponent::CelestiaAppCore BookmarkOrganizerParameter::AppCore() { return appCore; }
    CelestiaComponent::CelestiaRenderer BookmarkOrganizerParameter::Renderer() { return renderer;  }
    bool BookmarkOrganizerParameter::CanAddNewBookmarks() { return canAddNewBookmarks; }

    BookmarkOrganizerPage::BookmarkOrganizerPage() : appCore(nullptr), renderer(nullptr), canAddNewBookmarks(false)
    {
        bookmarks = single_threaded_observable_vector<BookmarkNode>();
        bindableBookmarks = BookmarkHelper::ConvertToBindable(bookmarks);
    }

    void BookmarkOrganizerPage::InitializeComponent()
    {
        BookmarkOrganizerPageT::InitializeComponent();
        NewFolderButton().Content(box_value(LocalizationHelper::Localize(L"New Folder", L"Create a folder in bookmark organizer")));
        DeleteButton().Content(box_value(LocalizationHelper::Localize(L"Delete", L"")));
        GoButton().Content(box_value(LocalizationHelper::Localize(L"Go", L"Go to an object")));
        RenameButton().Content(box_value(LocalizationHelper::Localize(L"Rename", L"Rename a favorite item (currently bookmark)")));
        EmptyHintText().Text(LocalizationHelper::Localize(L"No saved bookmarks", L""));
    }

    Windows::UI::Xaml::Interop::IBindableObservableVector BookmarkOrganizerPage::Bookmarks()
    {
        return bindableBookmarks;
    }

    IAsyncAction BookmarkOrganizerPage::WriteBookmarks()
    {
        if (isSaving) co_return;
        isSaving = true;
        co_await BookmarkHelper::WriteBookmarks(bookmarks);
        isSaving = false;
    }

    void BookmarkOrganizerPage::InsertBookmarkAtSelection(BookmarkNode const& bookmark)
    {
        const auto& [selectedBookmark, parentBookmark] = GetSelectionInfo();
        if (selectedBookmark == nullptr)
        {
            bookmarks.Append(bookmark); // no item selected, append to root
        }
        else if (selectedBookmark.IsFolder())
        {
            // add to selected folder, at the end
            selectedBookmark.Children().Append(bookmark);
        }
        else
        {
            // has item selected, append to above the selected item
            auto listToAddTo = parentBookmark == nullptr ? bookmarks : parentBookmark.Children();
            uint32_t index;
            if (listToAddTo.IndexOf(selectedBookmark, index))
            {
                listToAddTo.InsertAt(index, bookmark);
            }
            else
            {
                // should not happen
                listToAddTo.Append(bookmark);
            }
        }
        EmptyHintText().Visibility(Visibility::Collapsed);
        Tree().Visibility(Visibility::Visible);
    }

    fire_and_forget BookmarkOrganizerPage::SaveTimer_Tick(IInspectable const&, IInspectable const&)
    {
        co_await WriteBookmarks();
    }

    fire_and_forget BookmarkOrganizerPage::BookmarkOrganizerPage_Unloaded(IInspectable const&, RoutedEventArgs const&)
    {
        if (saveTimer != nullptr)
        {
            saveTimer.Tick(saveTimerToken);
            saveTimer.Stop();
        }
        if (isRead)
            co_await WriteBookmarks();
    }

    fire_and_forget BookmarkOrganizerPage::NewFolderButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        co_await CreateNewFolder();
    }

    IAsyncAction BookmarkOrganizerPage::CreateNewFolder()
    {
        auto name{ co_await ContentDialogHelper::GetText(*this, LocalizationHelper::Localize(L"Folder name", L"Enter name for new created folder")) };
        if (name.empty()) co_return;

        BookmarkNode bookmark{ true, name, L"", single_threaded_observable_vector<BookmarkNode>() };
        InsertBookmarkAtSelection(bookmark);
    }
   
    fire_and_forget BookmarkOrganizerPage::RenameButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        const auto& [bookmark, parentBookmark] = GetSelectionInfo();
        if (bookmark == nullptr) return;
        auto listToChangeIn = parentBookmark == nullptr ? bookmarks : parentBookmark.Children();
        auto bookmarkToChange = bookmark;

        auto name{ co_await ContentDialogHelper::GetText(*this, LocalizationHelper::Localize(L"New name", L"Enter new name for a bookmark node")) };
        if (name.empty()) co_return;

        uint32_t index;
        if (listToChangeIn.IndexOf(bookmarkToChange, index))
        {
            bookmarkToChange.Name(name);
            listToChangeIn.SetAt(index, bookmarkToChange); // Force the change to be reflected on UI
        }
    }

    void BookmarkOrganizerPage::DeleteButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        const auto& [bookmark, parentBookmark] = GetSelectionInfo();
        auto listToRemoveFrom = parentBookmark == nullptr ? bookmarks : parentBookmark.Children();

        uint32_t index;
        if (listToRemoveFrom.IndexOf(bookmark, index))
        {
            listToRemoveFrom.RemoveAt(index);
        }

        if (bookmarks.Size() == 0)
        {
            EmptyHintText().Visibility(Visibility::Visible);
            Tree().Visibility(Visibility::Collapsed);
        }
    }

    void BookmarkOrganizerPage::GoButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto selectedItem = Tree().SelectedItem();
        if (selectedItem == nullptr) return;
        auto bookmark = selectedItem.try_as<BookmarkNode>();
        if (bookmark == nullptr) return;
        if (bookmark.IsFolder() || bookmark.URL().empty()) return;
        renderer.EnqueueTask([this, bookmark]()
            {
                appCore.GoToURL(bookmark.URL());
            });
    }

    fire_and_forget BookmarkOrganizerPage::ReadBookmarks()
    {
        LoadingIndicator().Visibility(Visibility::Visible);
        EmptyHintText().Visibility(Visibility::Collapsed);
        Tree().Visibility(Visibility::Collapsed);
        auto newBookmarks = co_await BookmarkHelper::ReadBookmarks();
        bookmarks.ReplaceAll(std::vector<BookmarkNode>(newBookmarks.begin(), newBookmarks.end()));
        isRead = true;
        LoadingIndicator().Visibility(Visibility::Collapsed);
        if (bookmarks.Size() == 0)
            EmptyHintText().Visibility(Visibility::Visible);
        else
            Tree().Visibility(Visibility::Visible);
    }

    std::pair<BookmarkNode, BookmarkNode> BookmarkOrganizerPage::GetSelectionInfo()
    {
        auto selectedNode = Tree().SelectedNode();
        if (selectedNode == nullptr) return { nullptr, nullptr };
        auto bookmark = selectedNode.Content().try_as<BookmarkNode>();
        if (bookmark == nullptr) return { nullptr, nullptr };
        auto parentNode = selectedNode.Parent();
        if (parentNode == nullptr) return { bookmark, nullptr };
        return { bookmark, parentNode.Content().try_as<BookmarkNode>() };
    }

    void BookmarkOrganizerPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto parameter = e.Parameter().as<CelestiaUWP2::BookmarkOrganizerParameter>();
        appCore = parameter.AppCore();
        renderer = parameter.Renderer();
        canAddNewBookmarks = parameter.CanAddNewBookmarks();
        ReadBookmarks();
        saveTimer = DispatcherTimer();
        saveTimer.Interval(std::chrono::seconds(10));
        saveTimerToken = saveTimer.Tick({ get_weak(), &BookmarkOrganizerPage::SaveTimer_Tick });
        saveTimer.Start();
    }
}