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
    public delegate void ShowItemHandler(ResourceItem item);
    public delegate void GetAddonsHandler();

    public class InstalledListParameter
    {
        public ResourceManager ResourceManager;
        public ShowItemHandler ShowItemHandler;
        public GetAddonsHandler GetAddonsHandler;
        public InstalledListParameter(ResourceManager resourceManager, ShowItemHandler showItemHandler, GetAddonsHandler getAddonsHandler)
        {
            this.ShowItemHandler = showItemHandler;
            this.GetAddonsHandler = getAddonsHandler;
            this.ResourceManager = resourceManager;
        }
    }

    public class ResourceManagerPageParameter
    {
        public CelestiaAppCore AppCore;
        public CelestiaRenderer Renderer;
        public ResourceManager ResourceManager;
        public GetAddonsHandler GetAddonsHandler;

        public ResourceManagerPageParameter(CelestiaAppCore appCore, CelestiaRenderer renderer, ResourceManager resourceManager, GetAddonsHandler getAddonsHandler)
        {
            this.AppCore = appCore;
            this.Renderer = renderer;
            this.ResourceManager = resourceManager;
            this.GetAddonsHandler = getAddonsHandler;
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
            ResourceManager = parameter.ResourceManager;
            Container.Navigate(typeof(InstalledItemListPage), new InstalledListParameter(ResourceManager, delegate (ResourceItem item)
            {
                ShowItem(item);
            }, parameter.GetAddonsHandler));
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
