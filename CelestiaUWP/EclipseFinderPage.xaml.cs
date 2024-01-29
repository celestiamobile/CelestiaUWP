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

using CelestiaAppComponent;
using CelestiaComponent;
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
        private CelestiaAppCore AppCore;
        private CelestiaRenderer Renderer;

        private DateTime? StartTime = DateTime.Now;
        private DateTime? EndTime = DateTime.Now;

        private bool FindSolar = true;
        private bool FindLunar = true;

        private string objectPath = "";

        private readonly SearchObjectEntry[] AvailableObjects = new SearchObjectEntry[] { new SearchObjectEntry(CelestiaAppCore.LocalizedString("Earth", "celestia-data"), "Sol/Earth"), new SearchObjectEntry(CelestiaAppCore.LocalizedString("Jupiter", "celestia-data"), "Sol/Jupiter") };

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

        public EclipseFinderPage()
        {
            this.InitializeComponent();
            GoButton.Content = LocalizationHelper.Localize("Set Date and Go to Planet", "Set date to eclipse date and view eclipse");
            ComputeButton.Content = LocalizationHelper.Localize("Compute", "Compute for eclipses");
            StartTimeHint.Text = LocalizationHelper.Localize("Start time:", "In eclipse finder, range of time to find eclipse in");
            EndTimeHint.Text = LocalizationHelper.Localize("End time:", "In eclipse finder, range of time to find eclipse in");
            ObjectHint.Text = LocalizationHelper.Localize("Object:", "In Go to, Windows, In eclipse finder, object to find eclipse with");
            SolarEclipseButton.Content = LocalizationHelper.Localize("Solar", "Solar eclipses.");
            LunarEclipseButton.Content = LocalizationHelper.Localize("Lunar", "Lunar eclipses.");
            EmptyHintText.Text = LocalizationHelper.Localize("Find eclipses for an object in a time range", "");
            var obj = AvailableObjects[0];
            ObjectChooser.Text = obj.Name;
            objectPath = obj.Path;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = ((CelestiaAppCore, CelestiaRenderer))e.Parameter;
            AppCore = parameter.Item1;
            Renderer = parameter.Item2;
        }

        private async void ComputeButton_Click(object sender, RoutedEventArgs e)
        {
            if (Finder != null)
            {
                Finder.Abort();
                Finder = null;
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
                kind = CelestiaEclipseKind.SolarAndLunar;
            else if (FindSolar)
                kind = CelestiaEclipseKind.Solar;
            else
                kind = CelestiaEclipseKind.Lunar;

            var body = AppCore.Simulation.Find(objectPath).Object;
            if (body == null || !(body is CelestiaBody))
                return;

            ComputeButton.Content = LocalizationHelper.Localize("Cancel", "");
            EmptyHintText.Visibility = Visibility.Collapsed;
            LoadingIndicator.Visibility = Visibility.Visible;
            ResultList.Visibility = Visibility.Collapsed;
            var eclipses = await Compute((CelestiaBody)body, kind, startTime, endTime);
            Eclipses = eclipses;
            LoadingIndicator.Visibility = Visibility.Collapsed;
            if (eclipses.Length == 0)
            {
                EmptyHintText.Text = LocalizationHelper.Localize("No eclipse is found for the given object in the time range", "");
                EmptyHintText.Visibility = Visibility.Visible;
            }
            else
            {
                ResultList.Visibility = Visibility.Visible;
            }
            ComputeButton.Content = LocalizationHelper.Localize("Compute", "Compute for eclipses");
        }

        private async Task<CelestiaEclipse[]> Compute(CelestiaBody body, CelestiaEclipseKind kind, DateTime startTime, DateTime endTime)
        {
            var eclipseFinder = new CelestiaEclipseFinder(body);
            Finder = eclipseFinder;
            return await Task.Run(() =>
            {
                var eclipses = eclipseFinder.Search(kind, startTime, endTime);
                eclipseFinder.Dispose();
                Finder = null;
                return eclipses ?? (new CelestiaEclipse[] { });
            });
        }

        private void GoButton_Click(object sender, RoutedEventArgs e)
        {
            var selectedEclipse = ResultList.SelectedItem;
            if (selectedEclipse == null)
                return;

            var eclipse = (CelestiaEclipse)selectedEclipse;
            Renderer.EnqueueTask(() =>
            {
                AppCore.Simulation.GoToEclipse(eclipse);
            });
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
        static public string GetEclipseDescription(CelestiaBody occulter, CelestiaBody receiver)
        {
            return occulter.Name + " - " + receiver.Name;
        }

        private void ObjectChooser_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            if (args.Reason != AutoSuggestionBoxTextChangeReason.UserInput) return;
            objectPath = sender.Text;
        }

        private void ObjectChooser_SuggestionChosen(AutoSuggestBox sender, AutoSuggestBoxSuggestionChosenEventArgs args)
        {
            var selected = args.SelectedItem as SearchObjectEntry;
            objectPath = selected.Path;
            sender.Text = selected.Name;
        }
    }
}
