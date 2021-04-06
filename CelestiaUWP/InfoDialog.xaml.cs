//
// InfoDialog.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

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
