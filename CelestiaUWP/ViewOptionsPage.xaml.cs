using System;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Navigation;

using CelestiaComponent;

namespace CelestiaUWP
{
    public class FilterStarsConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            return (double)MathF.Log((float)value, 1000000f) * 10000;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            return (float)MathF.Pow(1000000f, (float)((double)value) / 10000f);
        }
    }
    public sealed partial class ViewOptionsPage : Page
    {
        private string[] InfoDescriptions = new string[]
        {
            "None", "Terse", "Verbose"
        };

        private CelestiaAppCore AppCore;

        public ViewOptionsPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            AppCore = e.Parameter as CelestiaAppCore;
        }
    }
}
