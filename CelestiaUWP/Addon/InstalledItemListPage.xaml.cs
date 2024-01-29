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
using System;
using System.ComponentModel;
using System.Linq;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Addon
{
    public sealed partial class InstalledItemListPage : Page, INotifyPropertyChanged
    {
        private ShowItemHandler ShowItemHandler;
        private GetAddonsHandler GetAddonsHandler;
        private ResourceManager ResourceManager;
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
            Title.Text = LocalizationHelper.Localize("Installed", "");
            EmptyHintText.Text = LocalizationHelper.Localize("Enhance Celestia with online add-ons", "");
            EmptyHintButton.Content = LocalizationHelper.Localize("Get Add-ons", "");
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = e.Parameter as InstalledListParameter;
            ShowItemHandler = parameter.ShowItemHandler;
            GetAddonsHandler = parameter.GetAddonsHandler;
            ResourceManager = parameter.ResourceManager;
            LoadItems();
        }

        private async void LoadItems()
        {
            LoadingIndicator.Visibility = Visibility.Visible;
            EmptyHint.Visibility = Visibility.Collapsed;
            ItemList.Visibility = Visibility.Collapsed;
            var items = await ResourceManager.InstalledItems();
            Items = items.ToArray();
            LoadingIndicator.Visibility = Visibility.Collapsed;
            if (Items.Length == 0)
                EmptyHint.Visibility = Visibility.Visible;
            else
                ItemList.Visibility = Visibility.Visible;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        private void ItemList_ItemClick(object sender, ItemClickEventArgs e)
        {
            var item = e.ClickedItem as ResourceItem;
            if (item == null) return;
            ShowItemHandler(item);
        }

        private void GetAddonButton_Click(object sender, RoutedEventArgs e)
        {
            GetAddonsHandler();
        }
    }
}
