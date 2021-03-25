using System;
using Windows.UI.Xaml.Controls;

namespace CelestiaUWP.Helper
{
    public class ContentDialogHelper
    {
        public static async void ShowAlert(Page page, string title)
        {
            var alert = new ContentDialog();
            alert.Title = title;
            alert.PrimaryButtonText = LocalizationHelper.Localize("OK");
            await alert.ShowAsync();
        }
    }
}
