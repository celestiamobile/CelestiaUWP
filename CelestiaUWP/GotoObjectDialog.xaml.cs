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

        public double? Latitude
        {
            get
            {
                double result;
                if (double.TryParse(LatitudeString, out result))
                    return result;
                return null;
            }
            set
            {
                LatitudeString = value == null ? "" : ((double)value).ToString();
            }
        }

        public double? Longitude
        {
            get {
                double result;
                if (double.TryParse(LongitudeString, out result))
                    return result;
                return null;
            }
            set {
                LongitudeString = value == null ? "" : ((double)value).ToString();
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

        ObservableCollection<String> Units = new ObservableCollection<String>(new String[] {
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
        }

        private void ContentDialog_PrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }

        private void ContentDialog_SecondaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }
    }
}
