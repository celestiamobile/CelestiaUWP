//
// InstalledItemListPage.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaAppComponent;
using System.ComponentModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Addon
{
    public sealed partial class InstalledItemListPage : Page, INotifyPropertyChanged
    {
        private ShowItemHandler Handler;

        private ResourceItem[] mItems = new ResourceItem[] { };

        ResourceItem[] Items
        {
            get => mItems;
            set
            {
                mItems = value;
                OnPropertyChanged("Items");
            }
        }

        public InstalledItemListPage()
        {
            this.InitializeComponent();
            Title.Text = LocalizationHelper.Localize("Installed");
        }
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = e.Parameter as InstalledListParameter;
            Handler = parameter.Handler;
            LoadItems();
        }

        private async void LoadItems()
        {
            Items = await ResourceManager.Shared.InstalledItems();
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private void ListView_SelectionChanged(object sender, TappedRoutedEventArgs e)
        {
            var source = e.OriginalSource as FrameworkElement;
            if (source == null) return;
            var item = source.DataContext as ResourceItem;
            if (item == null) return;
            Handler(item);
        }
    }
}
