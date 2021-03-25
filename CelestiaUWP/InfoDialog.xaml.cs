using CelestiaUWP.Helper;
using Windows.UI.Xaml.Controls;

namespace CelestiaUWP
{
    public sealed partial class InfoDialog : ContentDialog
    {
        string Text;

        public InfoDialog(string text)
        {
            this.InitializeComponent();
            PrimaryButtonText = LocalizationHelper.Localize("OK");
            Text = text;
        }

        private void ContentDialog_PrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }

        private void ContentDialog_SecondaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
        }
    }
}
