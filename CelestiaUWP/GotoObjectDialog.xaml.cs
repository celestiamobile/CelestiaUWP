//
// GotoObjectDialog.xaml.cs
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
using System.Threading.Tasks;
using Windows.Globalization.NumberFormatting;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;

namespace CelestiaUWP
{
    public sealed partial class GotoObjectDialog : ContentDialog
    {
        public string ObjectPath = "";

        private CelestiaAppCore appCore;
        private CelestiaRenderer renderer;

        private readonly DecimalFormatter NumberFormatter = new DecimalFormatter();

        public float Latitude
        {
            get => LatitudeValue ?? 0.0f;
        }
        public float Longitude
        {
            get => LongitudeValue ?? 0.0f;
        }

        public double Distance
        {
            get => DistanceValue ?? 8.0;
        }

        private float? LatitudeValue = 0.0f;
        private float? LongitudeValue = 0.0f;
        private double? DistanceValue = 8.0f;

        public int Unit = 1;
        private readonly string[] Units = new string[]
        {
            LocalizationHelper.Localize("km", "Unit"),
            LocalizationHelper.Localize("radii", "In Go to, specify the distance based on the object radius"),
            LocalizationHelper.Localize("au", "Astronomical unit")
        };

        public GotoObjectDialog(CelestiaAppCore appCore, CelestiaRenderer renderer)
        {
            NumberFormatter.FractionDigits = 0;
            NumberFormatter.IsGrouped = false;
            this.appCore = appCore;
            this.renderer = renderer;
            this.InitializeComponent();
            Title = LocalizationHelper.Localize("Go to Object", "");
            PrimaryButtonText = LocalizationHelper.Localize("OK", "");
            SecondaryButtonText = LocalizationHelper.Localize("Cancel", "");
            ObjectNameText.PlaceholderText = LocalizationHelper.Localize("Object Name", "");
            LongitudeText.Text = NumberFormatter.FormatDouble((double)Longitude);
            LatitudeText.Text = NumberFormatter.FormatDouble((double)Latitude);
            DistanceText.Text = NumberFormatter.FormatDouble(Distance);
            LongitudeText.PlaceholderText = LocalizationHelper.Localize("Longitude", "Coordinates");
            LatitudeText.PlaceholderText = LocalizationHelper.Localize("Latitude", "Coordinates");
            DistanceText.PlaceholderText = LocalizationHelper.Localize("Distance", "Distance to the object (in Go to)");
        }

        private void LongitudeText_TextChanged(object sender, TextChangedEventArgs args)
        {
            var value = NumberFormatter.ParseDouble(LongitudeText.Text);
            if (value != null)
            {
                var floatValue = (float)value;
                if (floatValue >= -180.0f && floatValue <= 180.0f)
                    LongitudeValue = floatValue;
                else
                    LongitudeValue = null;
            }
            else
            {
                LongitudeValue = null;
            }
            Validate();
        }

        private void LatitudeText_TextChanged(object sender, TextChangedEventArgs args)
        {
            var value = NumberFormatter.ParseDouble(LatitudeText.Text);
            if (value != null)
            {
                var floatValue = (float)value;
                if (floatValue >= -90.0f && floatValue <= 90.0f)
                    LatitudeValue = floatValue;
                else
                    LatitudeValue = null;
            }
            else
            {
                LatitudeValue = null;
            }
            Validate();
        }

        private void DistanceText_TextChanged(object sender, TextChangedEventArgs args)
        {
            var value = NumberFormatter.ParseDouble(DistanceText.Text);
            if (value != null)
            {
                var doubleValue = (double)value;
                if (doubleValue >= 0.0)
                    DistanceValue = doubleValue;
                else
                    DistanceValue = null;
            }
            else
            {
                DistanceValue = null;
            }
            Validate();
        }

        private async void ObjectNameText_TextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            var text = sender.Text;
            if (args.Reason != AutoSuggestionBoxTextChangeReason.UserInput) return;
            ObjectPath = text;

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
            ObjectPath = selected.Path;
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

        private void Validate()
        {
            if (LongitudeValue != null && LatitudeValue != null && DistanceValue != null)
                IsPrimaryButtonEnabled = true;
            else
                IsPrimaryButtonEnabled = false;
        }
    }
}
