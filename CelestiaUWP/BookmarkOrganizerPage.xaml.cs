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
using System;
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
            Unloaded += BookmarkOrganizerPage_Unloaded;
        }

        private void BookmarkOrganizerPage_Unloaded(object sender, RoutedEventArgs e)
        {
            WriteBookmarks();
        }

        public void InsertBookmarkAtSelection(BookmarkNode bookmark)
        {
            var (selected, parent) = GetSelectedBookmarkAndParent();
            if (selected == null)
            {
                Bookmarks.Add(bookmark);
            }
            else if (selected.IsFolder)
            {
                selected.Children.Add(bookmark);
            }
            else
            {
                if (parent == null)
                {
                    var listToAddTo = Bookmarks;
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
                else
                {
                    
                    var listToAddTo = parent.Children;
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
            }
            WriteBookmarks();
        }

        private void NewFolderButton_Click(object sender, RoutedEventArgs e)
        {
            CreateNewFolder();
        }

        private async void CreateNewFolder()
        {
            var text = await ContentDialogHelper.GetText(this, LocalizationHelper.Localize("Folder name"));
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
            var text = await ContentDialogHelper.GetText(this, LocalizationHelper.Localize("New name"));
            if (text.Length > 0)
            {
                if (parent == null)
                {
                    var index = Bookmarks.IndexOf(bookmark);
                    if (index >= 0)
                    {
                        bookmark.Name = text;
                        Bookmarks[index] = bookmark;
                        WriteBookmarks();
                    }
                }
                else
                {
                    var index = parent.Children.IndexOf(bookmark);
                    if (index >= 0)
                    {
                        bookmark.Name = text;
                        parent.Children[index] = bookmark;
                        WriteBookmarks();
                    }
                }
            }
        }

        private void DeleteButton_Click(object sender, RoutedEventArgs e)
        {
            var (bookmark, parent) = GetSelectedBookmarkAndParent();
            if (bookmark == null) return;

            if (parent == null)
                Bookmarks.Remove(bookmark);
            else
                parent.Children.Remove(bookmark);
            WriteBookmarks();
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
