//
// AboutDialog.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaAppComponent;
using System;
using Windows.ApplicationModel;
using Windows.Storage;
using Windows.UI.Xaml.Controls;

namespace CelestiaUWP
{
    public sealed partial class AboutDialog : ContentDialog
    {
        public AboutDialog(string authorFilePath, string translatorFilePath)
        {
            this.InitializeComponent();
            Title = LocalizationHelper.Localize("About Celestia");
            PrimaryButtonText = LocalizationHelper.Localize("OK");
            Package package = Package.Current;
            PackageId packageId = package.Id;
            PackageVersion version = packageId.Version;

            VersionLabel.Text = string.Format("Celestia {0}.{1}.{2}.{3}", version.Major, version.Minor, version.Build, version.Revision);

            LoadAuthorTranslator(authorFilePath, translatorFilePath);
        }

        private async void LoadAuthorTranslator(string authorFilePath, string translatorFilePath)
        {
            try
            {
                var authorFile = await StorageFile.GetFileFromPathAsync(authorFilePath);
                var content = await FileIO.ReadTextAsync(authorFile);
                AuthorLabel.Text = content;
                AuthorTitleLabel.Text = LocalizationHelper.Localize("Authors:");
                var tanslatorFile = await StorageFile.GetFileFromPathAsync(translatorFilePath);
                TranslatorLabel.Text = content;
                TranslatorTitleLabel.Text = LocalizationHelper.Localize("Translators:");
            }
            catch { };

        }
    }
}
