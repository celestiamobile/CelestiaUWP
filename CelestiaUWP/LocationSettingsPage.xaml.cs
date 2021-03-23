using System;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Navigation;

using CelestiaComponent;
using Windows.UI.Xaml;

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
            LocalizeElement(Content);
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            AppCore = e.Parameter as CelestiaAppCore;
        }

        private void LocalizeElement(UIElement element)
        {
            if (element is Panel)
            {
                foreach (var item in ((Panel)element).Children)
                {
                    LocalizeElement(item);
                }
            }
            else if (element is CheckBox)
            {
                var checkBox = (CheckBox)element;
                var content = checkBox.Content;
                if (content is string)
                    checkBox.Content = CelestiaAppCore.LocalizedString((string)content);
            }
            else if (element is TextBlock)
            {
                var textBlock = (TextBlock)element;
                textBlock.Text = CelestiaAppCore.LocalizedString(textBlock.Text);
            }
            else if (element is Slider)
            {
                var slider = (Slider)element;
                var header = slider.Header;
                if (header is string)
                    slider.Header = CelestiaAppCore.LocalizedString((string)header);
            }
        }
    }
}
