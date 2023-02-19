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

using CelestiaComponent;
using CelestiaUWP.Helper;
using System.Collections.Generic;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    public static class CelestiaExtension
    {
        public static CelestiaBrowserItem[] GetChildren(this CelestiaAppCore AppCore, CelestiaBrowserItem item)
        {
            var obj = item.Object;
            if (obj == null)
                return new CelestiaBrowserItem[] { };
            if (obj is CelestiaStar star)
                return AppCore.Simulation.Universe.ChildrenForStar(star, AppCore.GetChildren);
            if (obj is CelestiaBody body)
                return AppCore.Simulation.Universe.ChildrenForBody(body, AppCore.GetChildren);
            return new CelestiaBrowserItem[] { };
        }
    }
    public sealed partial class BrowserPage : Page, INotifyPropertyChanged
    {
        private CelestiaAppCore AppCore;
        private CelestiaRenderer Renderer;

        // Static ones, persistent
        private static CelestiaBrowserItem[] mSolRoot = null;
        private static CelestiaBrowserItem[] mDSORoot = null;
        private static CelestiaBrowserItem brightestStars = null;
        private static CelestiaBrowserItem starsWithPlanets = null;

        // Dynamic ones, updates every time the page is opened
        private CelestiaBrowserItem[] mStarRoot = null;

        private readonly Helper.NavigationViewItem[] NavigationItems = new Helper.NavigationViewItem[]
        {
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Solar System"), "sol"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Stars"), "star"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("DSOs"), "dso"),
        };
        private CelestiaBrowserItem[] Root
        {
            get => root;
            set
            {
                root = value;
                OnPropertyChanged("Root");
            }
        }

        private CelestiaBrowserItem[] root = new CelestiaBrowserItem[] { };

        public BrowserPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = ((CelestiaAppCore, CelestiaRenderer))e.Parameter;
            AppCore = parameter.Item1;
            Renderer = parameter.Item2;

            if (mSolRoot == null)
            {
                // Static should just initialize once
                var sol = AppCore.Simulation.Find("Sol");
                if (!sol.IsEmpty)
                {
                    var solStar = sol.Object;
                    if (solStar is CelestiaStar)
                    {
                        mSolRoot = new CelestiaBrowserItem[] { new CelestiaBrowserItem(AppCore.Simulation.Universe.StarCatalog.StarName((CelestiaStar)solStar), solStar, AppCore.GetChildren, false) };
                    }
                }
                else
                {
                    mSolRoot = new CelestiaBrowserItem[] { };
                }
            }

            if (brightestStars == null)
            {
                var bsb = AppCore.Simulation.StarBrowser(CelestiaStarBrowserType.Brightest);
                var brightest = bsb.Stars;
                bsb.Dispose();
                var s2 = new List<CelestiaBrowserItem>();
                foreach (var star in brightest)
                {
                    s2.Add(new CelestiaBrowserItem(AppCore.Simulation.Universe.StarCatalog.StarName(star), star, AppCore.GetChildren, true));
                }
                brightestStars = new CelestiaBrowserItem(LocalizationHelper.Localize("Brightest Stars (Absolute Magnitude)"), s2.ToArray(), true);
            }

            if (starsWithPlanets == null)
            {
                var hsb = AppCore.Simulation.StarBrowser(CelestiaStarBrowserType.WithPlants);
                var hasPlanets = hsb.Stars;
                hsb.Dispose();
                var s3 = new List<CelestiaBrowserItem>();
                foreach (var star in hasPlanets)
                {
                    s3.Add(new CelestiaBrowserItem(AppCore.Simulation.Universe.StarCatalog.StarName(star), star, AppCore.GetChildren, false));
                }
                starsWithPlanets = new CelestiaBrowserItem(LocalizationHelper.Localize("Stars with Planets"), s3.ToArray(), false);
            }

            var nsb = AppCore.Simulation.StarBrowser(CelestiaStarBrowserType.Nearest);
            var bsb2 = AppCore.Simulation.StarBrowser(CelestiaStarBrowserType.Brighter);
            var nearest = nsb.Stars;
            var brighter = bsb2.Stars;
            nsb.Dispose();
            bsb2.Dispose();

            var s1 = new List<CelestiaBrowserItem>();
            var s4 = new List<CelestiaBrowserItem>();
            foreach (var star in nearest)
            {
                s1.Add(new CelestiaBrowserItem(AppCore.Simulation.Universe.StarCatalog.StarName(star), star, AppCore.GetChildren, false));
            }
            foreach (var star in brighter)
            {
                s4.Add(new CelestiaBrowserItem(AppCore.Simulation.Universe.StarCatalog.StarName(star), star, AppCore.GetChildren, false));
            }
            mStarRoot = new CelestiaBrowserItem[]
            {
                new CelestiaBrowserItem(LocalizationHelper.Localize("Nearest Stars"), s1.ToArray(), true),
                new CelestiaBrowserItem(LocalizationHelper.Localize("Brightest Stars"), s4.ToArray(), true),
                brightestStars,
                starsWithPlanets
            };

            if (mDSORoot == null)
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
                    LocalizationHelper.Localize("Galaxies (Barred Spiral)"),
                    LocalizationHelper.Localize("Galaxies (Spiral)"),
                    LocalizationHelper.Localize("Galaxies (Elliptical)"),
                    LocalizationHelper.Localize("Galaxies (Irregular)"),
                    LocalizationHelper.Localize("Nebulae"),
                    LocalizationHelper.Localize("Globulars"),
                    LocalizationHelper.Localize("Open Clusters"),
                    LocalizationHelper.Localize("Unknown"),
                };
                var dsoCatalog = AppCore.Simulation.Universe.DSOCatalog;
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
                    var item = new CelestiaBrowserItem(dsoCatalog.DSOName(dso), dso, AppCore.GetChildren, false);
                    results[categoryIndex].Add(item);
                }
                var dsoCategories = new List<CelestiaBrowserItem>();
                for (var i = 0; i < results.Count; i++)
                {
                    if (results[i].Count > 0)
                    {
                        dsoCategories.Add(new CelestiaBrowserItem(categoryNames[i], results[i].ToArray(), false));
                    }
                }
                mDSORoot = dsoCategories.ToArray();
            }

            Root = mSolRoot;

            var actions = new (string, short)[] {
                    ("Go", 103),
                    ("Follow", 102),
                    ("Sync Orbit", 121),
                    ("Lock Phase", 58),
                    ("Chase", 34),
                    ("Track", 116)
                };
            foreach (var action in actions)
            {
                var button = new Button
                {
                    Content = LocalizationHelper.Localize(action.Item1)
                };
                button.Click += (sender, arg) =>
                {
                    var selectedItem = Tree.SelectedItem;
                    if (selectedItem is CelestiaBrowserItem item)
                    {
                        var obj = item.Object;
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
            Nav.SelectedItem = NavigationItems[0];
        }

        private void NavigationView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
        {
            if (mSolRoot == null)
                return;

            switch (((Helper.NavigationViewItem)args.SelectedItem).Tag)
            {
                case "sol":
                    Root = mSolRoot;
                    break;
                case "star":
                    Root = mStarRoot;
                    break;
                case "dso":
                    Root = mDSORoot;
                    break;
                default:
                    break;
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
