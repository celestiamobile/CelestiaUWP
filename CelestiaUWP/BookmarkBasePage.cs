using CelestiaComponent;
using Newtonsoft.Json;
using System.Collections.ObjectModel;
using System.IO;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

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
                var serializer = new JsonSerializer();
                using (var bookmarkStream = await localFolder.OpenStreamForReadAsync("bookmarks.json"))
                using (var sr = new StreamReader(bookmarkStream))
                using (var jsonTextReader = new JsonTextReader(sr))
                {
                    bookmarks = serializer.Deserialize<ObservableCollection<BookmarkNode>>(jsonTextReader);
                }
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
                var serializer = new JsonSerializer();
                using (var bookmarkStream = await localFolder.OpenStreamForWriteAsync("bookmarks.json", Windows.Storage.CreationCollisionOption.ReplaceExisting))
                using (var sr = new StreamWriter(bookmarkStream))
                using (var jsonTextWriter = new JsonTextWriter(sr))
                {
                    serializer.Serialize(jsonTextWriter, Bookmarks);
                }
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
