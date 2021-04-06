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

using CelestiaUWP.Helper;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Addon
{
    public sealed partial class InstalledItemListPage : Page, INotifyPropertyChanged
    {
        private ShowItemHandler Handler;

        private ResourceItem[] mItems = new ResourceItem[] { };

        ResourceItem[] Items
        {
            get { return mItems; }
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

        private void ListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Handler((ResourceItem)ItemList.SelectedItem);
        }
    }
}
