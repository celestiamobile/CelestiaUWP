//
// RendererSettingsPage.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaUWP.Helper;

namespace CelestiaUWP.Settings
{
    public sealed partial class RendererSettingsPage : SettingsBasePage
    {
        private readonly AppSettings AppSettings = AppSettings.Shared;

        private readonly string[] InfoDescriptions = new string[]
        {
            LocalizationHelper.Localize("None"),
            LocalizationHelper.Localize("Terse"),
            LocalizationHelper.Localize("Verbose")
        };
        private readonly string[] ResolutionDescriptions = new string[]
        {
            LocalizationHelper.Localize("Low"),
            LocalizationHelper.Localize("Medium"),
            LocalizationHelper.Localize("High")
        };
        private readonly string[] StarStyleDescriptions = new string[]
        {
            LocalizationHelper.Localize("Fuzzy Points"),
            LocalizationHelper.Localize("Points"),
            LocalizationHelper.Localize("Scaled Discs")
        };
        private readonly string[] TimeZoneDescriptions = new string[]
        {
            LocalizationHelper.Localize("Local Time"),
            LocalizationHelper.Localize("UTC"),
        };
        private readonly string[] TimeFormatDescriptions = new string[]
        {
            LocalizationHelper.Localize("Default"),
            LocalizationHelper.Localize("YYYY MMM DD HH:MM:SS TZ"),
            LocalizationHelper.Localize("UTC Offset")
        };

        public RendererSettingsPage()
        {
            this.InitializeComponent();
            LocalizeElement(Content);
        }
    }
}