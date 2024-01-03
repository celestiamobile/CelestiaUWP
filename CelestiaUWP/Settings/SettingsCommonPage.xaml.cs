//
// SettingsCommonPage.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaAppComponent;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Settings
{
    public class SettingsParameter
    {
        public SettingBaseItem[] Items;
        public bool ShowRestartHint;
        public SettingsParameter(SettingBaseItem[] items, bool showRestartHint)
        {
            this.Items = items;
            this.ShowRestartHint = showRestartHint;
        }
    }

    public class SettingsItemTemplateSelector : DataTemplateSelector
    {
        public DataTemplate Toggle { get; set; }
        public DataTemplate Slider { get; set; }
        public DataTemplate Selection { get; set; }
        public DataTemplate Header { get; set; }

        protected override DataTemplate SelectTemplateCore(object item)
        {
            if (item is SettingBooleanItem)
            {
                return Toggle;
            }
            else if (item is SettingDoubleItem)
            {
                return Slider;
            }
            else if (item is SettingInt32Item)
            {
                return Selection;
            }
            else if (item is SettingHeaderItem)
            {
                return Header;
            }
            else
            {
                return null;
            }
        }
    }
    public sealed partial class SettingsCommonPage : Page
    {
        private SettingBaseItem[] Items;
        private bool ShowRestartHint;

        public SettingsCommonPage()
        {
            this.InitializeComponent();
            RestartHint.Title = LocalizationHelper.Localize("Some configurations will take effect after a restart.");
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = (SettingsParameter)e.Parameter;
            Items = parameter.Items;
            ShowRestartHint = parameter.ShowRestartHint;
        }
    }
}
