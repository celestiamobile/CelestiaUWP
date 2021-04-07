//
// ViewOptionsPage.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaComponent;
using CelestiaUWP.Helper;
using CelestiaUWP.Settings;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    public sealed partial class ViewOptionsPage : Page, INotifyPropertyChanged
    {
        private Helper.NavigationViewItem[] NavigationItems = new Helper.NavigationViewItem[]
        {
            new Helper.NavigationViewItem(LocalizationHelper.Localize("General"), "general"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Guides"), "guides"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Labels"), "labels"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Renderer"), "renderer"),
        };

        private CelestiaAppCore AppCore;
        private readonly AppSettings AppSettings = AppSettings.Shared;

        public ViewOptionsPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            AppCore = e.Parameter as CelestiaAppCore;
            Nav.SelectedItem = NavigationItems[0];
        }

        private void Nav_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
        {
            if (args.SelectedItem == null) return;

            var item = (Helper.NavigationViewItem)args.SelectedItem;

            if (item.Tag == "general")
            {
                Container.Navigate(typeof(GeneralSettingsPage), AppCore);
            }
            else if (item.Tag == "guides")
            {
                Container.Navigate(typeof(GuidesSettingsPage), AppCore);
            }
            else if (item.Tag == "labels")
            {
                Container.Navigate(typeof(LabelsSettingsPage), AppCore);
            }
            else if (item.Tag == "renderer")
            {
                Container.Navigate(typeof(RendererSettingsPage), AppCore);
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
