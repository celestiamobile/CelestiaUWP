//
// EclipseFinderPage.xaml.cs
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
using System.ComponentModel;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    public sealed partial class EclipseFinderPage : Page, INotifyPropertyChanged
    {
        private CelestiaAppCore mAppCore;

        private DateTime? StartTime = DateTime.Now;
        private DateTime? EndTime = DateTime.Now;

        private bool FindSolar = true;
        private bool FindLunar = true;

        private int SelectedObjectIndex = 0;
        private readonly string[] AvailableObjects = new string[] { CelestiaAppCore.LocalizedString("Earth", "celestia"), CelestiaAppCore.LocalizedString("Jupiter", "celestia") };

        private CelestiaEclipseFinder Finder;
        private CelestiaEclipse[] Eclipses
        {
            get => eclipses;
            set
            {
                eclipses = value;
                OnPropertyChanged("Eclipses");
            }
        }

        private CelestiaEclipse[] eclipses = new CelestiaEclipse[] { };

        private int SelectedEclipseIndex = -1;

        public EclipseFinderPage()
        {
            this.InitializeComponent();
            GoButton.Content = LocalizationHelper.Localize("Set Date and Go to Planet");
            ComputeButton.Content = LocalizationHelper.Localize("Compute");
            StartTimeHint.Text = LocalizationHelper.Localize("Start time:");
            EndTimeHint.Text = LocalizationHelper.Localize("End time:");
            ObjectHint.Text = LocalizationHelper.Localize("Object:");
            SolarEclipseButton.Content = LocalizationHelper.Localize("Solar");
            LunarEclipseButton.Content = LocalizationHelper.Localize("Lunar");
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            mAppCore = e.Parameter as CelestiaAppCore;
        }

        private async void ComputeButton_Click(object sender, RoutedEventArgs e)
        {
            if (Finder != null)
            {
                Finder.Abort();
                Finder = null;
                ComputeButton.Content = LocalizationHelper.Localize("Compute");
                return;
            }
            if (StartTime == null || EndTime == null)
                return;
            var startTime = (DateTime)StartTime;
            var endTime = (DateTime)EndTime;
            if (endTime.CompareTo(startTime) <= 0)
                return;

            if (!FindLunar && !FindSolar)
                return;

            CelestiaEclipseKind kind;
            if (FindLunar && FindSolar)
                kind = CelestiaEclipseKind.solarAndLunar;
            else if (FindSolar)
                kind = CelestiaEclipseKind.solar;
            else
                kind = CelestiaEclipseKind.lunar;

            var body = mAppCore.Simulation.Find(AvailableObjects[SelectedObjectIndex]).Object;
            if (body == null || !(body is CelestiaBody))
                return;

            var eclipses = await Compute((CelestiaBody)body, kind, startTime, endTime);
            Eclipses = eclipses;
        }

        private async Task<CelestiaEclipse[]> Compute(CelestiaBody body, CelestiaEclipseKind kind, DateTime startTime, DateTime endTime)
        {
            var eclipseFinder = new CelestiaEclipseFinder(body);
            Finder = eclipseFinder;
            ComputeButton.Content = LocalizationHelper.Localize("Cancel");
            return await Task.Run<CelestiaEclipse[]>(() =>
            {
                var eclipses = eclipseFinder.Search(kind, startTime, endTime);
                return eclipses ?? (new CelestiaEclipse[] { });
            });
        }

        private void GoButton_Click(object sender, RoutedEventArgs e)
        {
            if (SelectedEclipseIndex < 0)
                return;

            var eclipse = Eclipses[SelectedEclipseIndex];
            mAppCore.Simulation.GoToEclipse(eclipse);
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        static public String GetEclipseDescription(CelestiaBody occulter, CelestiaBody receiver)
        {
            return occulter.Name + " - " + receiver.Name;
        }
    }
}
