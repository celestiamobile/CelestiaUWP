//
// SettingsBasePage.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaComponent;
using CelestiaUWP.Helper;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Settings
{
    public class SettingsBasePage : Page
    {
        protected CelestiaAppCore AppCore;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            AppCore = e.Parameter as CelestiaAppCore;
        }

        protected void LocalizeElement(UIElement element)
        {
            if (element is Panel panel)
            {
                foreach (var item in panel.Children)
                {
                    LocalizeElement(item);
                }
            }
            else if (element is CheckBox checkBox)
            {
                var content = checkBox.Content;
                if (content is string value)
                    checkBox.Content = LocalizationHelper.Localize(value);
                else if (content is TextBlock textBlock)
                    textBlock.Text = LocalizationHelper.Localize(textBlock.Text);
            }
            else if (element is TextBlock textBlock)
            {
                textBlock.Text = LocalizationHelper.Localize(textBlock.Text);
            }
            else if (element is Slider slider)
            {
                var header = slider.Header;
                if (header is string value)
                    slider.Header = LocalizationHelper.Localize(value);
            }
        }
    }
}
