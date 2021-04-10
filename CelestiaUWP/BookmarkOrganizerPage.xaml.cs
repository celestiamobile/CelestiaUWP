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

using CelestiaUWP.Helper;
using System;
using System.Collections.ObjectModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CelestiaUWP
{
    public sealed partial class BookmarkOrganizerPage : BookmarkBasePage
    {
        public BookmarkOrganizerPage()
        {
            this.InitializeComponent();

            NewFolderButton.Content = LocalizationHelper.Localize("New Folder");
            DeleteButton.Content = LocalizationHelper.Localize("Delete");
            GoButton.Content = LocalizationHelper.Localize("Go");
            RenameButton.Content = LocalizationHelper.Localize("Rename");
        }

        private void NewFolderButton_Click(object sender, RoutedEventArgs e)
        {
            CreateNewFolder();
        }

        private async void CreateNewFolder()
        {
            var dialog = new TextInputDialog(LocalizationHelper.Localize("Folder name"));
            var result = await dialog.ShowAsync();
            if (result == ContentDialogResult.Primary)
            {
                var bookmark = new BookmarkNode
                {
                    IsFolder = true,
                    Name = dialog.Text,
                    Children = new ObservableCollection<BookmarkNode>()
                };
                Bookmarks.Add(bookmark);
                WriteBookmarks();
            }
        }

        private void RenameButton_Click(object sender, RoutedEventArgs e)
        {
            if (Tree.SelectedItem == null) return;
            
            var node = (BookmarkNode)Tree.SelectedItem;
            RenameItem(node);
        }

        private async void RenameItem(BookmarkNode bookmark)
        {
            var dialog = new TextInputDialog(LocalizationHelper.Localize("New name"));
            var result = await dialog.ShowAsync();
            if (result == ContentDialogResult.Primary)
            {
                var parent = FindParent(bookmark, Bookmarks);
                if (parent != null)
                {
                    for (int i = 0; i < parent.Count; i++)
                    {
                        if (parent[i] == bookmark)
                        {
                            bookmark.Name = dialog.Text;
                            parent[i] = bookmark;
                            WriteBookmarks();
                        }
                    }
                }
            }
        }

        private void DeleteButton_Click(object sender, RoutedEventArgs e)
        {
            if (Tree.SelectedItem == null) return;

            var bookmark = (BookmarkNode)Tree.SelectedItem;
            var parent = FindParent(bookmark, Bookmarks);
            if (parent != null)
            {
                parent.Remove(bookmark);
                WriteBookmarks();
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
    }
}
