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

using CelestiaAppComponent;
using CelestiaComponent;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

using NavigationView = Microsoft.UI.Xaml.Controls.NavigationView;
using NavigationViewBackRequestedEventArgs = Microsoft.UI.Xaml.Controls.NavigationViewBackRequestedEventArgs;

namespace CelestiaUWP.Addon
{
    public delegate void ShowItemHandler(ResourceItem category);

    public class InstalledListParameter
    {
        public ShowItemHandler Handler;
        public ResourceManager ResourceManager;
        public InstalledListParameter(ShowItemHandler handler, ResourceManager resourceManager)
        {
            this.Handler = handler;
            this.ResourceManager = resourceManager;
        }
    }

    public class ResourceManagerPageParameter
    {
        public CelestiaAppCore AppCore;
        public CelestiaRenderer Renderer;
        public ResourceManager ResourceManager;
        public ResourceManagerPageParameter(CelestiaAppCore appCore, CelestiaRenderer renderer, ResourceManager resourceManager)
        {
            this.AppCore = appCore;
            this.Renderer = renderer;
            this.ResourceManager = resourceManager;
        }
    }

    public sealed partial class ResourceManagerPage : Page
    {
        private CelestiaAppCore AppCore;
        private CelestiaRenderer Renderer;
        private ResourceManager ResourceManager;
        public ResourceManagerPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = (ResourceManagerPageParameter)e.Parameter;
            AppCore = parameter.AppCore;
            Renderer = parameter.Renderer;
            ResourceManager= parameter.ResourceManager;
            Container.Navigate(typeof(InstalledItemListPage), new InstalledListParameter(delegate (ResourceItem item)
            {
                ShowItem(item);
            }, ResourceManager));
        }

        private void ShowItem(ResourceItem item)
        {
            Nav.IsBackEnabled = true;
            Container.Navigate(typeof(ResourceItemPage), new AddonPageParameter(AppCore, Renderer, item, ResourceManager));
        }

        private void Nav_BackRequested(NavigationView sender, NavigationViewBackRequestedEventArgs args)
        {
            if (!Container.CanGoBack) return;

            Container.GoBack();
            Nav.IsBackEnabled = Container.CanGoBack;
        }
    }
}
