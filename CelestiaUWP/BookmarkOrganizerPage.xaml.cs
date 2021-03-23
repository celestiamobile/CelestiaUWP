using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

using CelestiaComponent;
using System.Collections.ObjectModel;

namespace CelestiaUWP
{
    public sealed partial class BookmarkOrganizerPage : BookmarkBasePage
    {
        public BookmarkOrganizerPage()
        {
            this.InitializeComponent();
        }

        private void NewFolderButton_Click(object sender, RoutedEventArgs e)
        {
            CreateNewFolder();
        }

        private async void CreateNewFolder()
        {
            var dialog = new TextInputDialog(CelestiaAppCore.LocalizedString("Folder name"));
            var result = await dialog.ShowAsync();
            if (result == ContentDialogResult.Primary)
            {
                var text = dialog.Text;
                var bookmark = new BookmarkNode();
                bookmark.IsFolder = true;
                bookmark.Name = dialog.Text;
                bookmark.Children = new ObservableCollection<BookmarkNode>();
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
            var dialog = new TextInputDialog(CelestiaAppCore.LocalizedString("New name"));
            dialog.XamlRoot = XamlRoot;
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
                mAppCore.GoToURL(bookmark.URL);
        }
    }
}
