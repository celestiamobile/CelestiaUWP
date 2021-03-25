using CelestiaComponent;
using CelestiaUWP.Helper;
using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Navigation;

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
            LocalizationHelper.Localize("None"),
            LocalizationHelper.Localize("Terse"),
            LocalizationHelper.Localize("Verbose")
        };

        private CelestiaAppCore AppCore;

        public ViewOptionsPage()
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
                    checkBox.Content = LocalizationHelper.Localize((string)content);
            }
            else if (element is TextBlock)
            {
                var textBlock = (TextBlock)element;
                textBlock.Text = LocalizationHelper.Localize(textBlock.Text);
            }
            else if (element is Slider)
            {
                var slider = (Slider)element;
                var header = slider.Header;
                if (header is string)
                    slider.Header = LocalizationHelper.Localize((string)header);
            }
        }
    }
}
