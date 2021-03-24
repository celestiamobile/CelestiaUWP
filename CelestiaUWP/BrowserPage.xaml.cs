using System;
using System.Collections.Generic;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

using CelestiaComponent;
using System.ComponentModel;

namespace CelestiaUWP
{
    public class NavigationViewItem
    {
        public string Name;
        public string Tag;

        public NavigationViewItem(string Name, string Tag)
        {
            this.Name = Name;
            this.Tag = Tag;
        }
    }
    public sealed partial class BrowserPage : Page, INotifyPropertyChanged
    {
        private CelestiaAppCore mAppCore;
        private CelestiaBrowserItem[] mSolRoot;
        private CelestiaBrowserItem[] mStarRoot;
        private CelestiaBrowserItem[] mDSORoot;

        private NavigationViewItem[] NavigationItems = new NavigationViewItem[]
        {
            new NavigationViewItem(CelestiaAppCore.LocalizedString("Solar System"), "sol"),
            new NavigationViewItem(CelestiaAppCore.LocalizedString("Stars"), "star"),
            new NavigationViewItem(CelestiaAppCore.LocalizedString("DSOs"), "dso"),
        };
        private CelestiaBrowserItem[] mRoot
        {
            get { return root;  }
            set
            {
                root = value;
                OnPropertyChanged("mRoot");
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
                new CelestiaBrowserItem(CelestiaAppCore.LocalizedString("Nearest Stars"), s1.ToArray()),
                new CelestiaBrowserItem(CelestiaAppCore.LocalizedString("Brightest Stars"), s2.ToArray()),
                new CelestiaBrowserItem(CelestiaAppCore.LocalizedString("Stars with Planets"), s3.ToArray()),
            };

            var typeMap = new String[]
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
            var categoryNames = new String[]
            {
                CelestiaAppCore.LocalizedString("Galaxies (Barred Spiral)"),
                CelestiaAppCore.LocalizedString("Galaxies (Spiral)"),
                CelestiaAppCore.LocalizedString("Galaxies (Elliptical)"),
                CelestiaAppCore.LocalizedString("Galaxies (Irregular)"),
                CelestiaAppCore.LocalizedString("Nebulae"),
                CelestiaAppCore.LocalizedString("Globulars"),
                CelestiaAppCore.LocalizedString("Open Clusters"),
                CelestiaAppCore.LocalizedString("Unknown"),
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

            mRoot = mSolRoot;

            var actions = new (String, short)[] {
                    ("Go", 103),
                    ("Follow", 102),
                    ("Orbit Synchronously", 121),
                    ("Lock Phase", 58),
                    ("Chase", 34),
                    ("Track", 116)
                };
            foreach (var action in actions)
            {
                var button = new Button();
                button.Content = CelestiaAppCore.LocalizedString(action.Item1);
                button.Click += (sender, arg) =>
                {
                    var selectedItem = Tree.SelectedItem;
                    if (selectedItem is CelestiaBrowserItem)
                    {
                        var obj = ((CelestiaBrowserItem)selectedItem).Object;
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
            if (obj is CelestiaStar)
                return mAppCore.Simulation.Universe.ChildrenForStar((CelestiaStar)obj, GetChildren);
            if (obj is CelestiaBody)
                return mAppCore.Simulation.Universe.ChildrenForBody((CelestiaBody)obj, GetChildren);
            return new CelestiaBrowserItem[] { };
        }

        private void NavigationView_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
        {
            if (mSolRoot == null)
                return;

            switch (((NavigationViewItem)args.SelectedItem).Tag)
            {
                case "sol":
                    mRoot = mSolRoot;
                    break;
                case "star":
                    mRoot = mStarRoot;
                    break;
                case "dso":
                    mRoot = mDSORoot;
                    break;
                default:
                    break;
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
