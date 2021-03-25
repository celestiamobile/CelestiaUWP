using CelestiaComponent;
using CelestiaUWP.Helper;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
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
