using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

using CelestiaComponent;
using System.Runtime.Serialization.Json;
using System.ComponentModel;
using System.Collections.ObjectModel;

namespace CelestiaUWP
{
    public class BookmarkNode
    {
        public bool IsFolder;
        public string Name;
        public string URL;
        public ObservableCollection<BookmarkNode> Children;
    }

    public sealed partial class NewBookmarkPage : Page, INotifyPropertyChanged
    {
        private CelestiaAppCore mAppCore;
        private string NameText;

        private ObservableCollection<BookmarkNode> Bookmarks = new ObservableCollection<BookmarkNode>();
        public NewBookmarkPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            mAppCore = e.Parameter as CelestiaAppCore;
            readBookmarks();
        }
        async private void readBookmarks()
        {
            ObservableCollection<BookmarkNode> bookmarks;
            Windows.Storage.StorageFolder localFolder = Windows.Storage.ApplicationData.Current.LocalFolder;
            try
            {
                var path = localFolder.Path;
                var bookmarkStream = await localFolder.OpenStreamForReadAsync("bookmarks.json");
                var jsonSerializer = new DataContractJsonSerializer(typeof(ObservableCollection<BookmarkNode>));
                bookmarks = (ObservableCollection<BookmarkNode>)jsonSerializer.ReadObject(bookmarkStream);
                bookmarkStream.Close();
            }
            catch
            {
                bookmarks = new ObservableCollection<BookmarkNode>();
            }
            Bookmarks = bookmarks;
        }

        async public void writeBookmarks()
        {
            Windows.Storage.StorageFolder localFolder = Windows.Storage.ApplicationData.Current.LocalFolder;
            try
            {
                var bookmarkStream = await localFolder.OpenStreamForWriteAsync("bookmarks.json", Windows.Storage.CreationCollisionOption.ReplaceExisting);
                var jsonSerializer = new DataContractJsonSerializer(typeof(ObservableCollection<BookmarkNode>));
                jsonSerializer.WriteObject(bookmarkStream, Bookmarks);
                bookmarkStream.Close();
            }
            catch { }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private void NewFolderButton_Click(object sender, RoutedEventArgs e)
        {
            CreateNewFolder();
        }

        private async void CreateNewFolder()
        {
            var dialog = new TextInputDialog(CelestiaAppCore.LocalizedString("Folder name"));
            dialog.XamlRoot = XamlRoot;
            var result = await dialog.ShowAsync();
            if (result == ContentDialogResult.Primary)
            {
                var text = dialog.Text;
                var bookmark = new BookmarkNode();
                bookmark.IsFolder = true;
                bookmark.Name = dialog.Text;
                bookmark.Children = new ObservableCollection<BookmarkNode>();
                Bookmarks.Add(bookmark);
                writeBookmarks();
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
                            writeBookmarks();
                        }
                    }
                }
            }
        }

        private ObservableCollection<BookmarkNode> FindParent(BookmarkNode node, ObservableCollection<BookmarkNode> root)
        {
            foreach (var n in root)
            {
                if (n == node) return root;
                var p = FindParent(node, n.Children);
                if (p != null)
                    return p;
            }
            return null;
        }

        private void DeleteButton_Click(object sender, RoutedEventArgs e)
        {
            if (Tree.SelectedItem == null) return;

            var bookmark = (BookmarkNode)Tree.SelectedItem;
            var parent = FindParent(bookmark, Bookmarks);
            if (parent != null)
            {
                parent.Remove(bookmark);
                writeBookmarks();
            }
        }

        private void ConfirmButton_Click(object sender, RoutedEventArgs e)
        {
            if (NameText == null) return;
            var bookmark = new BookmarkNode();
            bookmark.IsFolder = false;
            bookmark.Name = NameText;
            bookmark.URL = mAppCore.CurrentURL;
            bookmark.Children = new ObservableCollection<BookmarkNode>();
            var parent = Tree.SelectedItem;
            if (parent == null)
                Bookmarks.Add(bookmark);
            else
                ((BookmarkNode)parent).Children.Add(bookmark);
            writeBookmarks();
        }
    }
}
