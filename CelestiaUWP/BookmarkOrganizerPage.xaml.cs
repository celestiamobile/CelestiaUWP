//
// BookmarkOrganizerPage.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaAppComponent;
using CelestiaComponent;
using System;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Interop;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    public class BookmarkOrganizerParameter
    {
        public CelestiaAppCore AppCore;
        public CelestiaRenderer Renderer;
        public bool CanAddNewBookmarks;
        public BookmarkOrganizerParameter(CelestiaAppCore appCore, CelestiaRenderer renderer, bool canAddNewBookmarks)
        {
            this.AppCore = appCore;
            this.Renderer = renderer;
            this.CanAddNewBookmarks = canAddNewBookmarks;
        }
    }

    public sealed partial class BookmarkOrganizerPage : Page
    {
        private CelestiaAppCore AppCore = null;
        private CelestiaRenderer Renderer = null;
        private bool CanAddNewBookmarks = false;

        private IObservableVector<BookmarkNode> bookmarks = BookmarkHelper.CreateEmptyList();
        private IBindableObservableVector Bookmarks = null;

        private DispatcherTimer saveTimer = null;
        private bool isSaving = false;
        private bool isRead = false;

        public BookmarkOrganizerPage()
        {
            Bookmarks = BookmarkHelper.ConvertToBindable(bookmarks);

            this.InitializeComponent();

            NewFolderButton.Content = LocalizationHelper.Localize("New Folder", "Create a folder in bookmark organizer");
            DeleteButton.Content = LocalizationHelper.Localize("Delete", "");
            GoButton.Content = LocalizationHelper.Localize("Go", "Go to an object");
            RenameButton.Content = LocalizationHelper.Localize("Rename", "Rename a favorite item (currently bookmark)");
            EmptyHintText.Text = LocalizationHelper.Localize("No saved bookmarks", "");
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = (BookmarkOrganizerParameter)e.Parameter;
            AppCore = parameter.AppCore;
            Renderer = parameter.Renderer;
            CanAddNewBookmarks = parameter.CanAddNewBookmarks;
            ReadBookmarks();
            saveTimer = new DispatcherTimer();
            saveTimer.Interval = TimeSpan.FromSeconds(10);
            saveTimer.Tick += SaveTimer_Tick;
            saveTimer.Start();
        }

        private void SaveTimer_Tick(object sender, object e)
        {
            WriteBookmarks();
        }

        private void BookmarkOrganizerPage_Unloaded(object sender, RoutedEventArgs e)
        {
            if (saveTimer != null)
            {
                saveTimer.Tick -= SaveTimer_Tick;
                saveTimer.Stop();
            }
            if (isRead)
                WriteBookmarks();
        }

        public void InsertBookmarkAtSelection(BookmarkNode bookmark)
        {
            var (selected, parent) = GetSelectedBookmarkAndParent();
            if (selected == null)
            {
                bookmarks.Add(bookmark);
            }
            else if (selected.IsFolder)
            {
                selected.Children.Add(bookmark);
            }
            else
            {
                var listToAddTo = parent == null ? bookmarks : parent.Children;
                var index = listToAddTo.IndexOf(selected);
                if (index >= 0)
                {
                    listToAddTo.Insert(index, bookmark);
                }
                else
                {
                    listToAddTo.Add(bookmark);
                }
            }
            EmptyHintText.Visibility = Visibility.Collapsed;
            Tree.Visibility = Visibility.Visible;
        }

        private void NewFolderButton_Click(object sender, RoutedEventArgs e)
        {
            CreateNewFolder();
        }
        private async void CreateNewFolder()
        {
            var text = await ContentDialogHelper.GetText(this, LocalizationHelper.Localize("Folder name", "Enter name for new created folder"));
            if (text.Length > 0)
            {
                var bookmark = new BookmarkNode(true, text, "", BookmarkHelper.CreateEmptyList());
                InsertBookmarkAtSelection(bookmark);
            }
        }
        private void RenameButton_Click(object sender, RoutedEventArgs e)
        {
            RenameItem();
        }
        private (BookmarkNode, BookmarkNode) GetSelectedBookmarkAndParent()
        {
            var selectedNode = Tree.SelectedNode;
            if (selectedNode == null) return (null, null);
            var bookmark = (BookmarkNode)selectedNode.Content;
            var parentNode = selectedNode.Parent;
            if (parentNode == null) return (bookmark, null);
            var parent = (BookmarkNode)parentNode.Content;
            return (bookmark, parent);
        }

        private async void RenameItem()
        {
            var (bookmark, parent) = GetSelectedBookmarkAndParent();
            if (bookmark == null) return;
            var text = await ContentDialogHelper.GetText(this, LocalizationHelper.Localize("New name", "Enter new name for a bookmark node"));
            if (text.Length <= 0) return;

            var listToChange = parent == null ? bookmarks : parent.Children;
            var index = bookmarks.IndexOf(bookmark);
            if (index >= 0)
            {
                bookmark.Name = text;
                listToChange[index] = bookmark;
            }
        }

        private void DeleteButton_Click(object sender, RoutedEventArgs e)
        {
            var (bookmark, parent) = GetSelectedBookmarkAndParent();
            if (bookmark == null) return;

            if (parent == null)
                bookmarks.Remove(bookmark);
            else
                parent.Children.Remove(bookmark);

            if (bookmarks.Count == 0)
            {
                EmptyHintText.Visibility = Visibility.Visible;
                Tree.Visibility = Visibility.Collapsed;
            }
        }

        private void GoButton_Click(object sender, RoutedEventArgs e)
        {
            if (Tree.SelectedItem == null) return;
            var bookmark = (BookmarkNode)Tree.SelectedItem;
            if (bookmark.URL != null)
            {
                Renderer.EnqueueTask(() =>
                {
                    AppCore.GoToURL(bookmark.URL);
                });
            }
        }

        async private void ReadBookmarks()
        {
            LoadingIndicator.Visibility = Visibility.Visible;
            EmptyHintText.Visibility = Visibility.Collapsed;
            Tree.Visibility = Visibility.Collapsed;
            bookmarks.Clear();
            var items = await BookmarkHelper.ReadBookmarks();
            foreach (var item in items)
            {
                bookmarks.Add(item);
            }
            isRead = true;
            LoadingIndicator.Visibility = Visibility.Collapsed;
            EmptyHintText.Visibility = bookmarks.Count == 0 ? Visibility.Visible : Visibility.Collapsed;
            Tree.Visibility = bookmarks.Count == 0 ? Visibility.Collapsed : Visibility.Visible;
        }

        async private void WriteBookmarks()
        {
            if (isSaving) return;
            isSaving = true;
            var vector = BookmarkHelper.CreateEmptyList();
            foreach (var bookmark in bookmarks)
            {
                vector.Add(bookmark);
            }
            await BookmarkHelper.WriteBookmarks(vector);
            isSaving = false;
        }
    }
}
