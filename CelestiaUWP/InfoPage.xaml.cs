//
// InfoPage.xaml.cs
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
using System;
using System.Collections.ObjectModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    public sealed partial class InfoPage : Page
    {
        private CelestiaAppCore AppCore;
        private CelestiaSelection Selection;
        private CelestiaRenderer Renderer;
        private ObservableCollection<BrowserInputAction> Actions = null;

        public InfoPage()
        {
            Actions = new ObservableCollection<BrowserInputAction>() {
                new BrowserInputAction(LocalizationHelper.Localize("Go", "Go to an object"), 103),
                new BrowserInputAction(LocalizationHelper.Localize("Follow", ""), 102),
                new BrowserInputAction(LocalizationHelper.Localize("Sync Orbit", ""), 121),
                new BrowserInputAction(LocalizationHelper.Localize("Lock Phase", ""), 58),
                new BrowserInputAction(LocalizationHelper.Localize("Chase", ""), 34),
                new BrowserInputAction(LocalizationHelper.Localize("Track", "Track an object"), 116)
            };
            this.InitializeComponent();
        }

        private void ActionButton_Click(object sender, RoutedEventArgs e)
        {
            var action = (sender as Button).DataContext as BrowserInputAction;
            Renderer.EnqueueTask(() =>
            {
                AppCore.Simulation.Selection = Selection;
                AppCore.CharEnter(action.Code);
            });
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = ((CelestiaAppCore, CelestiaRenderer, CelestiaSelection))e.Parameter;
            AppCore = parameter.Item1;
            Renderer = parameter.Item2;
            Selection = parameter.Item3;

            NameLabel.Text = AppCore.Simulation.Universe.NameForSelection(Selection);
            DetailLabel.Text = SelectionHelper.GetOverview(Selection, AppCore);
            var url = Selection.InfoURL;
            if (!string.IsNullOrEmpty(url))
            {
                LinkButton.NavigateUri = new Uri(url);
                LinkButton.Content = url;
                LinkButton.Visibility = Visibility.Visible;
            }
            else
            {
                LinkButton.Visibility = Visibility.Collapsed;
            }
        }
    }
}
