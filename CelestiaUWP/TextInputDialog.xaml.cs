//
// TextInputDialog.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaAppComponent;
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
    }
}
