// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "BookmarkOrganizerUserControl.xaml.h"
#if __has_include("BookmarkOrganizerUserControl.g.cpp")
#include "BookmarkOrganizerUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    BookmarkOrganizerUserControl::BookmarkOrganizerUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer)
    {
        bookmarks = single_threaded_observable_vector<BookmarkNode>();
        bindableBookmarks = BookmarkHelper::ConvertToBindable(bookmarks);
        InitializeComponent();
        NewFolderButton().Content(box_value(LocalizationHelper::Localize(L"New Folder")));
        DeleteButton().Content(box_value(LocalizationHelper::Localize(L"Delete")));
        GoButton().Content(box_value(LocalizationHelper::Localize(L"Go")));
        RenameButton().Content(box_value(LocalizationHelper::Localize(L"Rename")));
        ReadBookmarks();
        saveTimer = DispatcherTimer();
        saveTimer.Interval(std::chrono::seconds(10));
        saveTimerToken = saveTimer.Tick({ get_weak(), &BookmarkOrganizerUserControl::SaveTimer_Tick });
        saveTimer.Start();
    }

    Microsoft::UI::Xaml::Interop::IBindableObservableVector BookmarkOrganizerUserControl::Bookmarks()
    {
        return bindableBookmarks;
    }

    IAsyncAction BookmarkOrganizerUserControl::WriteBookmarks()
    {
        if (isSaving) co_return;
        isSaving = true;
        co_await BookmarkHelper::WriteBookmarks(bookmarks);
        isSaving = false;
    }

    void BookmarkOrganizerUserControl::InsertBookmarkAtSelection(BookmarkNode const& bookmark)
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
    }

    fire_and_forget BookmarkOrganizerUserControl::SaveTimer_Tick(IInspectable const&, IInspectable const&)
    {
        co_await WriteBookmarks();
    }

    fire_and_forget BookmarkOrganizerUserControl::BookmarkOrganizerPage_Unloaded(IInspectable const&, RoutedEventArgs const&)
    {
        if (saveTimer != nullptr)
        {
            saveTimer.Tick(saveTimerToken);
            saveTimer.Stop();
        }
        if (isRead)
            co_await WriteBookmarks();
    }

    fire_and_forget BookmarkOrganizerUserControl::NewFolderButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        co_await CreateNewFolder();
    }

    IAsyncAction BookmarkOrganizerUserControl::CreateNewFolder()
    {
        auto name{ co_await ContentDialogHelper::GetText(*this, LocalizationHelper::Localize(L"Folder name")) };
        if (name.empty()) co_return;

        BookmarkNode bookmark{ true, name, L"", single_threaded_observable_vector<BookmarkNode>() };
        InsertBookmarkAtSelection(bookmark);
    }
   
    fire_and_forget BookmarkOrganizerUserControl::RenameButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        const auto& [bookmark, parentBookmark] = GetSelectionInfo();
        if (bookmark == nullptr) return;
        auto listToChangeIn = parentBookmark == nullptr ? bookmarks : parentBookmark.Children();
        auto bookmarkToChange = bookmark;

        auto name{ co_await ContentDialogHelper::GetText(*this, LocalizationHelper::Localize(L"New name")) };
        if (name.empty()) co_return;

        uint32_t index;
        if (listToChangeIn.IndexOf(bookmarkToChange, index))
        {
            bookmarkToChange.Name(name);
            listToChangeIn.SetAt(index, bookmarkToChange); // Force the change to be reflected on UI
        }
    }

    void BookmarkOrganizerUserControl::DeleteButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        const auto& [bookmark, parentBookmark] = GetSelectionInfo();
        auto listToRemoveFrom = parentBookmark == nullptr ? bookmarks : parentBookmark.Children();

        uint32_t index;
        if (listToRemoveFrom.IndexOf(bookmark, index))
        {
            listToRemoveFrom.RemoveAt(index);
        }
    }

    void BookmarkOrganizerUserControl::GoButton_Click(IInspectable const&, RoutedEventArgs const&)
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

    fire_and_forget BookmarkOrganizerUserControl::ReadBookmarks()
    {
        auto newBookmarks = co_await BookmarkHelper::ReadBookmarks();
        bookmarks.ReplaceAll(std::vector<BookmarkNode>(newBookmarks.begin(), newBookmarks.end()));
        isRead = true;
    }

    std::pair<BookmarkNode, BookmarkNode> BookmarkOrganizerUserControl::GetSelectionInfo()
    {
        auto selectedNode = Tree().SelectedNode();
        if (selectedNode == nullptr) return { nullptr, nullptr };
        auto bookmark = selectedNode.Content().try_as<BookmarkNode>();
        if (bookmark == nullptr) return { nullptr, nullptr };
        auto parentNode = selectedNode.Parent();
        if (parentNode == nullptr) return { bookmark, nullptr };
        return { bookmark, parentNode.Content().try_as<BookmarkNode>() };
    }
}