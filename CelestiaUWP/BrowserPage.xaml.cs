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
using System;
using System.Collections.Generic;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    public sealed partial class BrowserPage : Page, INotifyPropertyChanged
    {
        private CelestiaAppCore mAppCore;
        private CelestiaBrowserItem[] mSolRoot;
        private CelestiaBrowserItem[] mStarRoot;
        private CelestiaBrowserItem[] mDSORoot;

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
            mAppCore = e.Parameter as CelestiaAppCore;
            var sol = mAppCore.Simulation.Find("Sol").Object as CelestiaStar;
            mSolRoot = new CelestiaBrowserItem[] { new CelestiaBrowserItem(mAppCore.Simulation.Universe.StarCatalog.StarName(sol), sol, GetChildren) };

            var nearest = mAppCore.Simulation.StarBrowser(CelestiaStarBrowserType.nearest).Stars;
            var brightest = mAppCore.Simulation.StarBrowser(CelestiaStarBrowserType.brightest).Stars;
            var hasPlanets = mAppCore.Simulation.StarBrowser(CelestiaStarBrowserType.withPlants).Stars;

            var s1 = new List<CelestiaBrowserItem>();
            var s2 = new List<CelestiaBrowserItem>();
            var s3 = new List<CelestiaBrowserItem>();
            foreach (var star in nearest)
            {
                s1.Add(new CelestiaBrowserItem(mAppCore.Simulation.Universe.StarCatalog.StarName(star), star, GetChildren));
            }
            foreach (var star in brightest)
            {
                s2.Add(new CelestiaBrowserItem(mAppCore.Simulation.Universe.StarCatalog.StarName(star), star, GetChildren));
            }
            foreach (var star in hasPlanets)
            {
                s3.Add(new CelestiaBrowserItem(mAppCore.Simulation.Universe.StarCatalog.StarName(star), star, GetChildren));
            }
            mStarRoot = new CelestiaBrowserItem[]
            {
                new CelestiaBrowserItem(LocalizationHelper.Localize("Nearest Stars"), s1.ToArray()),
                new CelestiaBrowserItem(LocalizationHelper.Localize("Brightest Stars"), s2.ToArray()),
                new CelestiaBrowserItem(LocalizationHelper.Localize("Stars with Planets"), s3.ToArray()),
            };

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
            var dsoCatalog = mAppCore.Simulation.Universe.DSOCatalog;
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
                var item = new CelestiaBrowserItem(dsoCatalog.DSOName(dso), dso, GetChildren);
                results[categoryIndex].Add(item);
            }
            var dsoCategories = new List<CelestiaBrowserItem>();
            for (var i = 0; i < results.Count; i++)
            {
                if (results[i].Count > 0)
                {
                    dsoCategories.Add(new CelestiaBrowserItem(categoryNames[i], results[i].ToArray()));
                }
            }
            mDSORoot = dsoCategories.ToArray();

            Root = mSolRoot;

            var actions = new (String, short)[] {
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
                            mAppCore.Simulation.Selection = new CelestiaSelection(obj);
                            mAppCore.CharEnter(action.Item2);
                        }
                    }
                };
                ButtonStack.Children.Add(button);
            }
            Nav.SelectedItem = NavigationItems[0];
        }

        private CelestiaBrowserItem[] GetChildren(CelestiaBrowserItem item)
        {
            var obj = item.Object;
            if (obj == null)
                return new CelestiaBrowserItem[] { };
            if (obj is CelestiaStar star)
                return mAppCore.Simulation.Universe.ChildrenForStar(star, GetChildren);
            if (obj is CelestiaBody body)
                return mAppCore.Simulation.Universe.ChildrenForBody(body, GetChildren);
            return new CelestiaBrowserItem[] { };
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
