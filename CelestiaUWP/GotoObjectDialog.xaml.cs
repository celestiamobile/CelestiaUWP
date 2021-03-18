using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

using CelestiaComponent;
using System.Collections.ObjectModel;

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
            CelestiaAppCore.LocalizedString("km"),
            CelestiaAppCore.LocalizedString("radii"),
            CelestiaAppCore.LocalizedString("au")
        });

        public GotoObjectDialog()
        {
            this.InitializeComponent();
            Title = CelestiaAppCore.LocalizedString("Go to Object");
            PrimaryButtonText = CelestiaAppCore.LocalizedString("OK");
            SecondaryButtonText = CelestiaAppCore.LocalizedString("Cancel");
        }

        private void ContentDialog_PrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }

        private void ContentDialog_SecondaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }
    }
}
