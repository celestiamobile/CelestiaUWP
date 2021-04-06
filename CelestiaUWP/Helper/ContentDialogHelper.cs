//
// ContentDialogHelper.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

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
