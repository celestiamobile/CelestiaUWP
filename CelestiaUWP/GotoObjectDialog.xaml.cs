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

namespace CelestiaUWP
{
    public sealed partial class GotoObjectDialog : ContentDialog
    {
        public string Text
        {
            get => ObjectNameText.Text;
        }

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
            LocalizationHelper.Localize("km"),
            LocalizationHelper.Localize("radii"),
            LocalizationHelper.Localize("au")
        };

        public GotoObjectDialog(CelestiaAppCore appCore, CelestiaRenderer renderer)
        {
            NumberFormatter.FractionDigits = 0;
            NumberFormatter.IsGrouped = false;
            this.appCore = appCore;
            this.renderer = renderer;
            this.InitializeComponent();
            Title = LocalizationHelper.Localize("Go to Object");
            PrimaryButtonText = LocalizationHelper.Localize("OK");
            SecondaryButtonText = LocalizationHelper.Localize("Cancel");
            ObjectNameText.PlaceholderText = LocalizationHelper.Localize("Object Name");
            LongitudeText.Text = NumberFormatter.FormatDouble((double)Longitude);
            LatitudeText.Text = NumberFormatter.FormatDouble((double)Latitude);
            DistanceText.Text = NumberFormatter.FormatDouble(Distance);
            LongitudeText.PlaceholderText = LocalizationHelper.Localize("Longitude");
            LatitudeText.PlaceholderText = LocalizationHelper.Localize("Latitude");
            DistanceText.PlaceholderText = LocalizationHelper.Localize("Distance");
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
            if (args.Reason != AutoSuggestionBoxTextChangeReason.UserInput) return;

            var text = sender.Text;
            if (text == "")
            {
                sender.ItemsSource = new string[] { };
                return;
            }

            var results = await GetCompletion(sender.Text);
            if (sender.Text != text) return;

            sender.ItemsSource = results;
        }

        private async Task<string[]> GetCompletion(string key)
        {
            var simulation = appCore.Simulation;
            var promise = new TaskCompletionSource<string[]>();
            renderer.EnqueueTask(() =>
            {
                var result = simulation.GetCompletion(key) ?? (new string[] { });
                promise.SetResult(result);
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
