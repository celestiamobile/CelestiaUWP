using System;
using System.IO;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
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

    public class BookmarkBasePage : Page
    {
        protected CelestiaAppCore mAppCore;
        protected ObservableCollection<BookmarkNode> Bookmarks = new ObservableCollection<BookmarkNode>();
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            mAppCore = e.Parameter as CelestiaAppCore;
            ReadBookmarks();
        }

        async private void ReadBookmarks()
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

        async public void WriteBookmarks()
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
        protected ObservableCollection<BookmarkNode> FindParent(BookmarkNode node, ObservableCollection<BookmarkNode> root)
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
    }
}
