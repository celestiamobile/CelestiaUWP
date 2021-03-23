using System;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Navigation;

using CelestiaComponent;

namespace CelestiaUWP
{
    public class DoubleFloatConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            return (double)((float)value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return (float)((double)value);
        }
    }

    public sealed partial class LocationSettingsPage : Page
    {
        private CelestiaAppCore AppCore;

        public LocationSettingsPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            AppCore = e.Parameter as CelestiaAppCore;
        }
    }
}
