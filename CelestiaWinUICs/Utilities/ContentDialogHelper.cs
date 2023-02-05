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

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Threading.Tasks;

namespace CelestiaWinUICs.Utilities
{
    public class ContentDialogHelper
    {
        public static async void ShowAlert(Window window, string title)
        {
            var alert = new ContentDialog
            {
                Title = title,
                PrimaryButtonText = LocalizationHelper.Localize("OK")
            };
            await alert.ShowAsync();
        }
        public static async Task<bool> ShowOption(Window window, string title)
        {
            var alert = new ContentDialog
            {
                Title = title,
                PrimaryButtonText = LocalizationHelper.Localize("OK"),
                SecondaryButtonText = LocalizationHelper.Localize("Cancel")
            };
            var result = await alert.ShowAsync();
            return result == ContentDialogResult.Primary;
        }
    }
}
