//
// ResourceManagerPage.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaComponent;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Addon
{
    public delegate void ShowItemHandler(ResourceItem category);

    public class InstalledListParameter
    {
        public ShowItemHandler Handler;
        public InstalledListParameter(ShowItemHandler handler)
        {
            this.Handler = handler;
        }
    }

    public sealed partial class ResourceManagerPage : Page
    {
        private CelestiaAppCore AppCore;
        private CelestiaRenderer Renderer;
        public ResourceManagerPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = ((CelestiaAppCore, CelestiaRenderer, string))e.Parameter;
            AppCore = parameter.Item1;
            Renderer = parameter.Item2;
            ResourceManager.Shared.AddonFolderPath = parameter.Item3;
            Container.Navigate(typeof(InstalledItemListPage), new InstalledListParameter((ShowItemHandler)delegate (ResourceItem item)
            {
                ShowItem(item);
            }));
        }

        private void ShowItem(ResourceItem item)
        {
            Nav.IsBackEnabled = true;
            Container.Navigate(typeof(ResourceItemPage), (AppCore, Renderer, item));
        }

        private void Nav_BackRequested(NavigationView sender, NavigationViewBackRequestedEventArgs args)
        {
            if (!Container.CanGoBack) return;

            Container.GoBack();
            Nav.IsBackEnabled = Container.CanGoBack;
        }
    }
}
