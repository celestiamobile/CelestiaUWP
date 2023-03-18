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

using CelestiaAppComponent;
using CelestiaComponent;
using CelestiaUWP.Helper;
using CelestiaUWP.Web;
using System;
using System.ComponentModel;
using System.IO;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Addon
{
    public class AddonPageParameter
    {
        public CelestiaAppCore AppCore;
        public CelestiaRenderer Renderer;
        public ResourceItem Item;
        public ResourceManager ResourceManager;
        public AddonPageParameter(CelestiaAppCore appCore, CelestiaRenderer renderer, ResourceItem item, ResourceManager resourceManager)
        {
            this.AppCore = appCore;
            this.Renderer = renderer;
            this.Item = item;
            this.ResourceManager = resourceManager;
        }
    }

    public sealed partial class ResourceItemPage : Page, INotifyPropertyChanged
    {
        private CelestiaAppCore AppCore;
        private CelestiaRenderer Renderer;

        private ResourceItem mItem;
        private ResourceManager ResourceManager;
        ResourceItem Item
        {
            get => mItem;
            set
            {
                mItem = value;
                OnPropertyChanged("Item");
            }
        }

        ResourceItemState State
        {
            get
            {
                return ResourceManager.StateForItem(Item);
            }
        }

        public ResourceItemPage()
        {
            this.InitializeComponent();
            GoButton.Content = LocalizationHelper.Localize("Go");
            ActionButton.Content = LocalizationHelper.Localize("Install");
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = (AddonPageParameter)e.Parameter;
            AppCore = parameter.AppCore;
            Renderer = parameter.Renderer;
            Item = parameter.Item;
            ResourceManager = parameter.ResourceManager;

            ResourceManager.DownloadProgressUpdate += Shared_ProgressUpdate;
            ResourceManager.DownloadSuccess += Shared_DownloadSuccess;
            ResourceManager.DownloadFailure += Shared_DownloadFailure;

            UpdateState();
            ReloadItem();

            bool isXbox = Windows.System.Profile.AnalyticsInfo.VersionInfo.DeviceFamily == "Windows.Xbox";

            var queryItems = System.Web.HttpUtility.ParseQueryString("");
            queryItems.Add("lang", LocalizationHelper.Locale);
            queryItems.Add("item", Item.ID);
            queryItems.Add("platform", isXbox ? "xbox" : "uwp");
            queryItems.Add("titleVisibility", "visible");
            queryItems.Add("transparentBackground", "1");
            var builder = new UriBuilder("https://celestia.mobi/resources/item");
            builder.Query = queryItems.ToString();
            var args = new CommonWebArgs();
            args.Renderer = Renderer;
            args.AppCore = AppCore;
            args.Uri = builder.Uri;
            args.MatchingQueryKeys = new string[] { "item" };
            args.ContextDirectory = ResourceManager.ItemPath(Item);
            WebContent.Navigate(typeof(SafeWebPage), args);
        }

        private void Shared_DownloadFailure(object sender, ResourceManagerDownloadFailureArgs args)
        {
            if (args.Item.ID != mItem.ID) return;
            UpdateState();
        }

        private void Shared_DownloadSuccess(object sender, ResourceManagerDownloadSuccessArgs args)
        {
            if (args.Item.ID != mItem.ID) return;
            UpdateState();
        }

        private void Shared_ProgressUpdate(object sender, ResourceManagerDownloadProgressArgs args)
        {
            if (args.Item.ID != mItem.ID) return;
            InstallProgressBar.Value = args.Progress;
        }

        private void UpdateState()
        {
            GoButton.Content = LocalizationHelper.Localize(Item.Type == "script" ? "Run" :"Go");

            switch (State)
            {
                case ResourceItemState.Downloading:
                    InstallProgressBar.Visibility = Visibility.Visible;
                    ActionButton.Content = LocalizationHelper.Localize("Cancel");
                    break;
                case ResourceItemState.Installed:
                    InstallProgressBar.Visibility = Visibility.Collapsed;
                    ActionButton.Content = LocalizationHelper.Localize("Uninstall");
                    break;
                case ResourceItemState.None:
                    InstallProgressBar.Visibility = Visibility.Collapsed;
                    ActionButton.Content = LocalizationHelper.Localize("Install");
                    break;
            }

            if (Item.Type == "script")
            {
                var mainScriptName = Item.MainScriptName;
                if (State == ResourceItemState.Installed && mainScriptName != null)
                {
                    var path = ResourceManager.ItemPath(Item) + "\\" + mainScriptName;
                    if (File.Exists(path))
                    {
                        GoButton.Visibility = Visibility.Visible;
                    }
                    else
                    {
                        GoButton.Visibility = Visibility.Collapsed;
                    }
                }
                else
                {
                    GoButton.Visibility = Visibility.Collapsed;
                }
            }
            else
            {
                if (State == ResourceItemState.Installed && Item.DemoObjectName != null && !AppCore.Simulation.Find(Item.DemoObjectName).IsEmpty)
                {
                    GoButton.Visibility = Visibility.Visible;
                }
                else
                {
                    GoButton.Visibility = Visibility.Collapsed;
                }
            }
        }

        private async void ReloadItem()
        {
            Windows.Web.Http.HttpClient httpClient = new Windows.Web.Http.HttpClient();
            var queryItems = System.Web.HttpUtility.ParseQueryString("");
            queryItems.Add("lang", LocalizationHelper.Locale);
            queryItems.Add("item", Item.ID);
            var builder = new UriBuilder(Addon.Constants.APIPrefix + "/resource/item");
            builder.Query = queryItems.ToString();
            try
            {
                var httpResponse = await httpClient.GetAsync(builder.Uri);
                httpResponse.EnsureSuccessStatusCode();
                var httpResponseBody = await httpResponse.Content.ReadAsStringAsync();
                var requestResult = RequestResult.TryParse(httpResponseBody);
                if (requestResult.Status == 0)
                {
                    var item = ResourceItem.TryParse(requestResult.Info.Detail);
                    Item = item;
                    UpdateState();
                }
            }
            catch { }
        }

        private async void ActionButton_Click(object sender, RoutedEventArgs e)
        {
            switch (State)
            {
                case ResourceItemState.Downloading:
                    ResourceManager.Cancel(Item);
                    UpdateState();
                    break;
                case ResourceItemState.Installed:
                    await ResourceManager.Uninstall(Item);
                    UpdateState();
                    break;
                case ResourceItemState.None:
                    ResourceManager.Download(Item);
                    UpdateState();
                    break;
            }
        }

        private async void GoButton_Click(object sender, RoutedEventArgs e)
        {
            if (Item.Type == "script")
            {
                var mainScriptName = Item.MainScriptName;
                if (mainScriptName == null) return;
                var path = ResourceManager.ItemPath(Item) + "\\" + mainScriptName;
                if (File.Exists(path))
                {
                    Renderer.EnqueueTask(() =>
                    {
                        AppCore.RunScript(path);
                    });
                }
                return;
            }
            var objectName = Item.DemoObjectName;
            if (objectName == null) return;

            var selection = AppCore.Simulation.Find(Item.DemoObjectName);
            if (selection.IsEmpty)
            {
                await ContentDialogHelper.ShowAlert(this, LocalizationHelper.Localize("Object not found."));
                return;
            }

            Renderer.EnqueueTask(() =>
            {
                AppCore.Simulation.Selection = selection;
                AppCore.CharEnter(103);
            });
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
