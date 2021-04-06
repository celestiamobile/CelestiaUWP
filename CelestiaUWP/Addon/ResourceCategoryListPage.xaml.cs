//
// ResourceCategoryListPage.xaml.cs
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
    public sealed partial class ResourceCategoryListPage : Page, INotifyPropertyChanged
    {
        private ShowCategoryHandler Handler;

        private ResourceCategory[] mCategories = new ResourceCategory[] { };

        ResourceCategory[] Categories
        {
            get => mCategories;
            set
            {
                mCategories = value;
                OnPropertyChanged("Categories");
            }
        }

        public ResourceCategoryListPage()
        {
            this.InitializeComponent();
            Title.Text = LocalizationHelper.Localize("Categories");
        }
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            Handler = e.Parameter as ShowCategoryHandler;
            LoadCategories();
        }

        private async void LoadCategories()
        {
            Windows.Web.Http.HttpClient httpClient = new Windows.Web.Http.HttpClient();
            Uri requestUri = new Uri(Constants.APIPrefix + "/resource/categories?lang=" + LocalizationHelper.Locale);
            try
            {
                var httpResponse = await httpClient.GetAsync(requestUri);
                httpResponse.EnsureSuccessStatusCode();
                var httpResponseBody = await httpResponse.Content.ReadAsStringAsync();
                var categories = JsonConvert.DeserializeObject<RequestResult>(httpResponseBody).Get<ResourceCategory[]>();
                Categories = categories;
            }
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
            Handler((ResourceCategory)CategoryList.SelectedItem);
        }
    }
}
