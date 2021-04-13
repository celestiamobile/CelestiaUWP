//
// GeneralSettingsPage.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaUWP.Helper;
using System;

namespace CelestiaUWP.Settings
{
    public sealed partial class GeneralSettingsPage : SettingsBasePage
    {
        private AppSettings AppSettings;
        private string[] AvailableLanguages;

        public GeneralSettingsPage()
        {
            this.InitializeComponent();
            LocalizeElement(Content);
        }

        protected override void OnNavigatedTo(SettingsArgs args)
        {
            base.OnNavigatedTo(args);
            var obj = ((AppSettings, string[]))args.Object;
            AppSettings = obj.Item1;
            AvailableLanguages = obj.Item2;

            LanguageComboBox.Items.Add("System");
            foreach (var language in AvailableLanguages)
            {
                var lang = language;
                if (lang == "zh_CN")
                    lang = "zh-Hans";
                else if (lang == "zh_TW")
                    lang = "zh-Hant";
                else
                    lang = lang.Replace("_", "-");
                var culture = new System.Globalization.CultureInfo(lang);
                LanguageComboBox.Items.Add(culture.NativeName);
            }

            var selectedLang = AppSettings.LanguageOverride;
            var selectedIndex = Array.IndexOf(AvailableLanguages, selectedLang);
            LanguageComboBox.SelectedIndex = selectedIndex + 1;
        }

        private void LanguageComboBox_SelectionChanged(object sender, Windows.UI.Xaml.Controls.SelectionChangedEventArgs e)
        {
            if (LanguageComboBox.SelectedIndex == 0)
            {
                AppSettings.LanguageOverride = null;
            }
            else
            {
                AppSettings.LanguageOverride = AvailableLanguages[LanguageComboBox.SelectedIndex - 1];
            }
        }
    }
}
