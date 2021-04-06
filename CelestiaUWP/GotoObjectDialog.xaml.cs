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

using CelestiaUWP.Helper;
using System;
using System.Collections.ObjectModel;
using Windows.UI.Xaml.Controls;

namespace CelestiaUWP
{
    public sealed partial class GotoObjectDialog : ContentDialog
    {
        public string Text = "";
        public string LongitudeString = "";
        public string LatitudeString = "";
        public string DistanceString = "";

        public float? Latitude
        {
            get
            {
                float result;
                if (float.TryParse(LatitudeString, out result))
                    return result;
                return null;
            }
            set
            {
                LatitudeString = value == null ? "" : ((float)value).ToString();
            }
        }

        public float? Longitude
        {
            get {
                float result;
                if (float.TryParse(LongitudeString, out result))
                    return result;
                return null;
            }
            set {
                LongitudeString = value == null ? "" : ((float)value).ToString();
            }
        }

        public double? Distance
        {
            get
            {
                double result;
                if (double.TryParse(DistanceString, out result))
                    return result;
                return null;
            }
            set
            {
                DistanceString = value == null ? "" : ((double)value).ToString();
            }
        }

        public int Unit = 0;

        ObservableCollection<string> Units = new ObservableCollection<string>(new string[] {
            LocalizationHelper.Localize("km"),
            LocalizationHelper.Localize("radii"),
            LocalizationHelper.Localize("au")
        });

        public GotoObjectDialog()
        {
            this.InitializeComponent();
            Title = LocalizationHelper.Localize("Go to Object");
            PrimaryButtonText = LocalizationHelper.Localize("OK");
            SecondaryButtonText = LocalizationHelper.Localize("Cancel");
            LongitudeText.PlaceholderText = LocalizationHelper.Localize("Longitude");
            LatitudeText.PlaceholderText = LocalizationHelper.Localize("Latitude");
            DistanceText.PlaceholderText = LocalizationHelper.Localize("Distance");
        }

        private void ContentDialog_PrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }

        private void ContentDialog_SecondaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }
    }
}
