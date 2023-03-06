//
// BookmarkBasePage.cs
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
using System.Collections.ObjectModel;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    public class BookmarkBasePage : Page, INotifyPropertyChanged
    {
        protected CelestiaAppCore AppCore;
        protected CelestiaRenderer Renderer;

        protected ObservableCollection<BookmarkNode> Bookmarks = new ObservableCollection<BookmarkNode>();
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = ((CelestiaAppCore, CelestiaRenderer))e.Parameter;
            AppCore = parameter.Item1;
            Renderer = parameter.Item2;
            ReadBookmarks();
        }

        async private void ReadBookmarks()
        {
            Bookmarks.Clear();
            var bookmarks = new ObservableCollection<BookmarkNode>();
            var items = await BookmarkHelper.ReadBookmarks();
            foreach (var item in items)
            {
                bookmarks.Add(item);
            }
            Bookmarks = bookmarks;
            OnPropertyChanged("Bookmarks");
        }

        async public void WriteBookmarks()
        {
            var vector = BookmarkHelper.CreateEmptyList();
            foreach (var bookmark in Bookmarks)
            {
                vector.Add(bookmark);
            }
            await BookmarkHelper.WriteBookmarks(vector);
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
