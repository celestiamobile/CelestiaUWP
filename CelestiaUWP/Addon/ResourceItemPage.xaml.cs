//
// ResourceItemPage.xaml.cs
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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Addon
{
    public class ImageConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)

        {
            if (value == null)
            {
                BitmapImage blankimage = new BitmapImage();
                return blankimage;
            }
            else
            {
                return new BitmapImage(new Uri(value.ToString()));
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }

    public sealed partial class ResourceItemPage : Page, INotifyPropertyChanged
    {
        private ResourceItem mItem;
        ResourceItem Item
        {
            get => mItem;
            set
            {
                mItem = value;
                OnPropertyChanged("Item");
            }
        }

        ResourceManager.ItemState State
        {
            get
            {
                return ResourceManager.Shared.StateForItem(Item);
            }
        }

        public ResourceItemPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var param = e.Parameter as ItemParameter;
            Item = param.Item;

            ResourceManager.Shared.ProgressUpdate += Shared_ProgressUpdate;
            ResourceManager.Shared.DownloadSuccess += Shared_DownloadSuccess;
            ResourceManager.Shared.DownloadFailure += Shared_DownloadFailure;

            UpdateState();
            ReloadItem();
        }

        private void Shared_DownloadFailure(ResourceItem item)
        {
            UpdateState();
        }

        private void Shared_DownloadSuccess(ResourceItem item)
        {
            UpdateState();
        }

        private void Shared_ProgressUpdate(ResourceItem item, double progress)
        {
            if (item.id != Item.id) return;
            ProgressText.Text = String.Format("{0:0.00}%", progress);
        }

        private void UpdateState()
        {
            switch (State)
            {
                case ResourceManager.ItemState.Downloading:
                    ActionButton.Content = LocalizationHelper.Localize("Cancel");
                    break;
                case ResourceManager.ItemState.Installed:
                    ProgressText.Text = "";
                    ActionButton.Content = LocalizationHelper.Localize("Uninstall");
                    break;
                case ResourceManager.ItemState.None:
                    ProgressText.Text = "";
                    ActionButton.Content = LocalizationHelper.Localize("Install");
                    break;
            }
        }

        private async void ReloadItem()
        {
            Windows.Web.Http.HttpClient httpClient = new Windows.Web.Http.HttpClient();
            var queryItems = System.Web.HttpUtility.ParseQueryString("");
            queryItems.Add("lang", LocalizationHelper.Locale);
            queryItems.Add("item", Item.id);
            var builder = new UriBuilder(Addon.Constants.APIPrefix + "/resource/item");
            builder.Query = queryItems.ToString();
            try
            {
                var httpResponse = await httpClient.GetAsync(builder.Uri);
                httpResponse.EnsureSuccessStatusCode();
                var httpResponseBody = await httpResponse.Content.ReadAsStringAsync();
                var item = JsonConvert.DeserializeObject<RequestResult>(httpResponseBody).Get<ResourceItem>();
                Item = item;
            }
            catch (Exception ex)
            {
                ContentDialogHelper.ShowAlert(this, ex.Message);
            }
        }

        private void ActionButton_Click(object sender, RoutedEventArgs e)
        {
            switch (State)
            {
                case ResourceManager.ItemState.Downloading:
                    ResourceManager.Shared.Cancel(Item);
                    UpdateState();
                    break;
                case ResourceManager.ItemState.Installed:
                    ResourceManager.Shared.Uninstall(Item);
                    UpdateState();
                    break;
                case ResourceManager.ItemState.None:
                    ResourceManager.Shared.DownloadItem(Item);
                    UpdateState();
                    break;
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
    }
}
