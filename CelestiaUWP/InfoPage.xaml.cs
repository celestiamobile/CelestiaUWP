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
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    public sealed partial class InfoPage : Page
    {
        public InfoPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = ((CelestiaAppCore, CelestiaSelection))e.Parameter;
            var AppCore = parameter.Item1;
            var Selection = parameter.Item2;

            NameLabel.Text = AppCore.Simulation.Universe.NameForSelection(Selection);
            DetailLabel.Text = SelectionHelper.GetOverview(Selection, AppCore);
            var url = Selection.InfoURL;
            if (!string.IsNullOrEmpty(url))
            {
                LinkButton.NavigateUri = new Uri(url);
                LinkButton.Content = url;
            }
        }
    }
}
