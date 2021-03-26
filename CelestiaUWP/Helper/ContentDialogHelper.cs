using System;
using System.Threading.Tasks;
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
        public static async Task<bool> ShowOption(Page page, string title)
        {
            var alert = new ContentDialog();
            alert.Title = title;
            alert.PrimaryButtonText = LocalizationHelper.Localize("OK");
            alert.SecondaryButtonText = LocalizationHelper.Localize("Cancel");
            var result = await alert.ShowAsync();
            return result == ContentDialogResult.Primary;
        }
    }
}
