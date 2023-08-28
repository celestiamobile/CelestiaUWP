//
// ObserverModeDialog.xaml.cs
//
// Copyright © 2023 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaComponent;
using CelestiaAppComponent;
using System;
using System.ComponentModel;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Documents;

namespace CelestiaUWP
{
    public class ObserverModeStringConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if (value is CoordinateSystem val)
            {
                switch (val)
                {
                case CoordinateSystem.Universal:
                    return LocalizationHelper.Localize("Free Flight");
                case CoordinateSystem.Ecliptical:
                    return LocalizationHelper.Localize("Follow");
                case CoordinateSystem.BodyFixed:
                    return LocalizationHelper.Localize("Sync Orbit");
                case CoordinateSystem.PhaseLock:
                    return LocalizationHelper.Localize("Phase Lock");
                case CoordinateSystem.Chase:
                    return LocalizationHelper.Localize("Chase");
                }
            }
            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return null;
        }
    }

    public sealed partial class ObserverModeDialog : ContentDialog, INotifyPropertyChanged
    {
        private readonly CelestiaAppCore appCore;
        private readonly CelestiaRenderer renderer;

        readonly CoordinateSystem[] CoordinateSystems = new CoordinateSystem[] { CoordinateSystem.Universal, CoordinateSystem.Ecliptical, CoordinateSystem.BodyFixed, CoordinateSystem.PhaseLock, CoordinateSystem.Chase };

        private CoordinateSystem selectedCoordinateSystem = CoordinateSystem.Universal;

        public string ReferenceObjectPath
        {
            get { return ReferenceNameText.Visibility == Visibility.Visible ? referenceObjectPath : ""; }
        }
        public string TargetObjectPath
        {
            get { return TargetNameText.Visibility == Visibility.Visible ? targetObjectPath : ""; }
        }
        private string referenceObjectPath = "";
        private string targetObjectPath = "";

        public CoordinateSystem SelectedCoordinateSystem
        {
            get => selectedCoordinateSystem;
            set
            {
                selectedCoordinateSystem = value;
                OnPropertyChanged(nameof(RefObjectTextBoxVisibility));
                OnPropertyChanged(nameof(TargetObjectTextBoxVisibility));
            }
        }

        public Visibility RefObjectTextBoxVisibility => SelectedCoordinateSystem == CoordinateSystem.Universal ? Visibility.Collapsed : Visibility.Visible;
        public Visibility TargetObjectTextBoxVisibility => SelectedCoordinateSystem == CoordinateSystem.PhaseLock ? Visibility.Visible : Visibility.Collapsed;

        public ObserverModeDialog(CelestiaAppCore appCore, CelestiaRenderer renderer)
        {
            this.appCore = appCore;
            this.renderer = renderer;
            this.InitializeComponent();
            CoordinateSystemHint.Text = LocalizationHelper.Localize("Coordinate System:");
            ReferenceNameText.PlaceholderText = LocalizationHelper.Localize("Reference Object");
            TargetNameText.PlaceholderText = LocalizationHelper.Localize("Target Object");
            var infoText = LocalizationHelper.Localize("Flight mode decides how you move around in Celestia. Learn more…");
            var infoLinkText = LocalizationHelper.Localize("Learn more…");
            var linkTextPos = infoText.IndexOf(infoLinkText);
            if (linkTextPos == -1)
            {
                LearnMoreHint.Visibility= Visibility.Collapsed;
            }
            else
            {
                var textBefore = infoText.Substring(0, linkTextPos);
                var textAfter = infoText.Substring(linkTextPos + infoLinkText.Length);
                var span = new Span();
                span.Inlines.Add(new Run() { Text = textBefore });

                var link = new Hyperlink();
                link.Inlines.Add(new Run() { Text = infoLinkText });
                link.NavigateUri = new Uri($"https://celestia.mobi/help/flight-mode?lang={LocalizationHelper.Locale}");
                span.Inlines.Add(link);

                span.Inlines.Add(new Run() { Text = textAfter });
                LearnMoreHint.Inlines.Add(span);
            }

            PrimaryButtonText = LocalizationHelper.Localize("OK");
            SecondaryButtonText = LocalizationHelper.Localize("Cancel");
        }

        private async void ObjectNameText_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            var text = sender.Text;
            var isReference = sender == ReferenceNameText;
            if (args.Reason != AutoSuggestionBoxTextChangeReason.UserInput) return;
            if (isReference)
                referenceObjectPath = text;
            else
                targetObjectPath = text;


            if (text == "")
            {
                sender.ItemsSource = new SearchObjectEntry[] { };
                return;
            }

            var results = await GetCompletion(sender.Text);
            if (sender.Text != text) return;

            sender.ItemsSource = results;
        }

        private void ObjectNameText_SuggestionChosen(AutoSuggestBox sender, AutoSuggestBoxSuggestionChosenEventArgs args)
        {
            var selected = args.SelectedItem as SearchObjectEntry;
            var isReference = sender == ReferenceNameText;
            if (isReference)
                referenceObjectPath = selected.Path;
            else
                targetObjectPath = selected.Path;
            sender.Text = selected.Name;
        }

        private async Task<SearchObjectEntry[]> GetCompletion(string key)
        {
            var simulation = appCore.Simulation;
            var promise = new TaskCompletionSource<SearchObjectEntry[]>();
            renderer.EnqueueTask(() =>
            {
                var lastSeparatorPosition = key.LastIndexOf('/');
                var prefix = "";
                if (lastSeparatorPosition != -1)
                    prefix = key.Substring(0, lastSeparatorPosition + 1);
                var completions = simulation.GetCompletion(key) ?? (new string[] { });
                var results = new SearchObjectEntry[completions.Length];
                for (int i = 0; i < completions.Length; i++)
                {
                    results[i] = new SearchObjectEntry(completions[i], prefix + completions[i]);
                }
                promise.SetResult(results);
            });
            return await promise.Task;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
