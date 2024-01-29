//
// BrowserPage.xaml.cs
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
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Interop;
using Windows.UI.Xaml.Navigation;

using NavigationView = Microsoft.UI.Xaml.Controls.NavigationView;
using NavigationViewSelectionChangedEventArgs = Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs;
using NavigationViewBackRequestedEventArgs = Microsoft.UI.Xaml.Controls.NavigationViewBackRequestedEventArgs;

namespace CelestiaUWP
{
    public sealed partial class BrowserPage : Page, INotifyPropertyChanged
    {
        private CelestiaAppCore AppCore;
        private CelestiaRenderer Renderer;

        // Static ones, persistent
        private static BrowserItem[] SolRoot = null;
        private static BrowserItem[] DSORoot = null;
        private static BrowserItem BrightestStars = null;

        // Dynamic ones, updates every time the page is opened
        private BrowserItem[] StarRoot = null;

        private ObservableCollection<BrowserItemTab> RootItems = new ObservableCollection<BrowserItemTab>();
        private IBindableObservableVector RootItem = null;

        public BrowserPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = ((CelestiaAppCore, CelestiaRenderer))e.Parameter;
            AppCore = parameter.Item1;
            Renderer = parameter.Item2;

            var simulation = AppCore.Simulation;
            var universe = simulation.Universe;
            var observer = simulation.ActiveObserver;

            if (SolRoot == null)
            {
                // Static should just initialize once
                var sol = AppCore.Simulation.Find("Sol");
                if (!sol.IsEmpty)
                {
                    var solStar = sol.Object;
                    if (solStar is CelestiaStar)
                    {
                        SolRoot = new BrowserItem[] { new BrowserItem(new CelestiaBrowserItem(universe.StarCatalog.StarName((CelestiaStar)solStar), solStar, (CelestiaBrowserItem item) => { return CelestiaExtension.GetChildren(item, AppCore); }, false)) };
                    }
                }
            }
            if (SolRoot != null)
                RootItems.Add(new BrowserItemTab(SolRoot, LocalizationHelper.Localize("Solar System", "Tab for solar system in Star Browser")));

            if (BrightestStars == null)
            {
                var bsb = universe.StarBrowser(CelestiaStarBrowserType.Brightest, observer);
                var brightest = bsb.Stars;
                bsb.Dispose();
                var s = new List<CelestiaBrowserItem>();
                foreach (var star in brightest)
                {
                    s.Add(new CelestiaBrowserItem(universe.StarCatalog.StarName(star), star, (CelestiaBrowserItem item) => { return CelestiaExtension.GetChildren(item, AppCore); }, true));
                }
                BrightestStars = new BrowserItem(new CelestiaBrowserItem(LocalizationHelper.Localize("Brightest Stars (Absolute Magnitude)", ""), s.ToArray(), true));
            }

            var nsb = universe.StarBrowser(CelestiaStarBrowserType.Nearest, observer);
            var bsb2 = universe.StarBrowser(CelestiaStarBrowserType.Brighter, observer);
            var hsb = universe.StarBrowser(CelestiaStarBrowserType.WithPlants, observer);
            var nearest = nsb.Stars;
            var brighter = bsb2.Stars;
            var hasPlanets = hsb.Stars;
            nsb.Dispose();
            bsb2.Dispose();
            hsb.Dispose();

            var s1 = new List<CelestiaBrowserItem>();
            var s2 = new List<CelestiaBrowserItem>();
            var s3 = new List<CelestiaBrowserItem>();
            foreach (var star in nearest)
            {
                s1.Add(new CelestiaBrowserItem(universe.StarCatalog.StarName(star), star, (CelestiaBrowserItem item) => { return CelestiaExtension.GetChildren(item, AppCore); }, false));
            }
            foreach (var star in brighter)
            {
                s2.Add(new CelestiaBrowserItem(universe.StarCatalog.StarName(star), star, (CelestiaBrowserItem item) => { return CelestiaExtension.GetChildren(item, AppCore); }, false));
            }
            foreach (var star in hasPlanets)
            {
                s3.Add(new CelestiaBrowserItem(universe.StarCatalog.StarName(star), star, (CelestiaBrowserItem item) => { return CelestiaExtension.GetChildren(item, AppCore); }, false));
            }
            StarRoot = new BrowserItem[]
            {
                new BrowserItem(new CelestiaBrowserItem(LocalizationHelper.Localize("Nearest Stars", ""), s1.ToArray(), true)),
                new BrowserItem(new CelestiaBrowserItem(LocalizationHelper.Localize("Brightest Stars", ""), s2.ToArray(), true)),
                BrightestStars,
                new BrowserItem(new CelestiaBrowserItem(LocalizationHelper.Localize("Stars with Planets", ""), s3.ToArray(), true)),
            };
            RootItems.Add(new BrowserItemTab(StarRoot, LocalizationHelper.Localize("Stars", "Tab for stars in Star Browser")));

            if (DSORoot == null)
            {
                var typeMap = new string[]
                {
                    "SB",
                    "S",
                    "E",
                    "Irr",
                    "Neb",
                    "Glob",
                    "Open cluster",
                    "Unknown"
                };
                var results = new List<List<CelestiaBrowserItem>>();
                for (int i = 0; i < typeMap.Length; i++)
                {
                    results.Add(new List<CelestiaBrowserItem>());
                }
                var categoryNames = new string[]
                {
                    LocalizationHelper.Localize("Galaxies (Barred Spiral)", ""),
                    LocalizationHelper.Localize("Galaxies (Spiral)", ""),
                    LocalizationHelper.Localize("Galaxies (Elliptical)", ""),
                    LocalizationHelper.Localize("Galaxies (Irregular)", ""),
                    LocalizationHelper.Localize("Nebulae", ""),
                    LocalizationHelper.Localize("Globulars", ""),
                    LocalizationHelper.Localize("Open Clusters", ""),
                    LocalizationHelper.Localize("Unknown", ""),
                };
                var dsoCatalog = universe.DSOCatalog;
                for (int i = 0; i < dsoCatalog.Count; i++)
                {
                    var dso = dsoCatalog.DSOAt(i);
                    var categoryIndex = typeMap.Length - 1;
                    for (int j = 0; j < typeMap.Length; j++)
                    {
                        if (dso.Type.StartsWith(typeMap[j]))
                        {
                            categoryIndex = j;
                            break;
                        }
                    }
                    var entry = new CelestiaBrowserItem(dsoCatalog.DSOName(dso), dso, (CelestiaBrowserItem item) => { return CelestiaExtension.GetChildren(item, AppCore); }, false);
                    results[categoryIndex].Add(entry);
                }
                var dsoCategories = new List<BrowserItem>();
                for (var i = 0; i < results.Count; i++)
                {
                    if (results[i].Count > 0)
                    {
                        dsoCategories.Add(new BrowserItem(new CelestiaBrowserItem(categoryNames[i], results[i].ToArray(), false)));
                    }
                }
                DSORoot = dsoCategories.ToArray();
            }
            RootItems.Add(new BrowserItemTab(DSORoot, LocalizationHelper.Localize("DSOs", "Tab for deep sky objects in Star Browser")));
            var getInfoButton = new Button
            {
                Content = LocalizationHelper.Localize("Get Info", "Action for getting info about current selected object")
            };
            getInfoButton.Click += GetInfoButton_Click;
            ButtonStack.Children.Add(getInfoButton);
            var actions = new (string, short)[]
                {
                    (LocalizationHelper.Localize("Go", "Go to an object"), 103),
                    (LocalizationHelper.Localize("Follow", ""), 102),
                    (LocalizationHelper.Localize("Sync Orbit", ""), 121),
                    (LocalizationHelper.Localize("Lock Phase", ""), 58),
                    (LocalizationHelper.Localize("Chase", ""), 34),
                    (LocalizationHelper.Localize("Track", "Track an object"), 116)
                };
            foreach (var action in actions)
            {
                var button = new Button
                {
                    Content = action.Item1
                };
                button.Click += (sender, arg) =>
                {
                    var selectedItem = Tree.SelectedItem;
                    if (selectedItem is BrowserItem item)
                    {
                        var obj = item.Item.Object;
                        if (obj != null)
                        {
                            Renderer.EnqueueTask(() =>
                            {
                                var selection = new CelestiaSelection(obj);
                                AppCore.Simulation.Selection = selection;
                                AppCore.CharEnter(action.Item2);
                            });
                        }
                    }
                };
                ButtonStack.Children.Add(button);
            }

            Nav.SelectedItem = RootItems[0];
        }

        private void GetInfoButton_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            var selectedItem = Tree.SelectedItem;
            if (selectedItem is BrowserItem item)
            {
                var obj = item.Item.Object;
                if (obj != null)
                {
                    var selection = new CelestiaSelection(obj);
                    Container.Navigate(typeof(InfoPage), (AppCore, selection));
                    Nav.IsBackEnabled = true;
                }
            }
        }

        private void NavigationView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
        {
            var item = args.SelectedItem;
            if (item == null) return;
            var browserItem = item as BrowserItemTab;
            if (browserItem == null) return;
            RootItem = BrowserItem.ConvertToBindable(browserItem.Children);
            OnPropertyChanged("RootItem");
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        private void Nav_BackRequested(NavigationView sender, NavigationViewBackRequestedEventArgs args)
        {
            Container.Content = BrowserItemListContainer;
            Nav.IsBackEnabled = false;
        }
    }
}
