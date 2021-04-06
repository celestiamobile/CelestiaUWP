//
// ResourceItemListPage.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaUWP.Helper;
using Newtonsoft.Json;
using System;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Addon
{
    public sealed partial class ResourceItemListPage : Page, INotifyPropertyChanged
    {
        private ShowItemHandler Handler;
        private ResourceCategory Category;

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


        public ResourceItemListPage()
        {
            this.InitializeComponent();
        }
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = e.Parameter as CategoryParameter;
            Handler = parameter.Handler;
            Category = parameter.Category;
            LoadItems();
        }

        private async void LoadItems()
        {
            Windows.Web.Http.HttpClient httpClient = new Windows.Web.Http.HttpClient();
            Uri requestUri = new Uri(Constants.APIPrefix + "/resource/items?lang=" + LocalizationHelper.Locale + "&category=" + Category.id);
            try
            {
                var httpResponse = await httpClient.GetAsync(requestUri);
                httpResponse.EnsureSuccessStatusCode();
                var httpResponseBody = await httpResponse.Content.ReadAsStringAsync();
                var items = JsonConvert.DeserializeObject<RequestResult>(httpResponseBody).Get<ResourceItem[]>();
                Items = items;            }
            catch (Exception ex)
            {
                ContentDialogHelper.ShowAlert(this, ex.Message);
            }
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
