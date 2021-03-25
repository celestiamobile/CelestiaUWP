using CelestiaUWP.Helper;
using Windows.UI.Xaml.Controls;

namespace CelestiaUWP
{
    public sealed partial class TextInputDialog : ContentDialog
    {
        public string Text = "";

        public TextInputDialog(string title)
        {
            this.InitializeComponent();
            Title = title;
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
