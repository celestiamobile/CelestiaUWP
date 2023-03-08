//
// WelcomeDialog.xaml.cs
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
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Documents;

namespace CelestiaUWP
{
    public sealed partial class WelcomeDialog : ContentDialog
    {
        public WelcomeDialog()
        {
            this.InitializeComponent();
            PrimaryButtonText = LocalizationHelper.Localize("OK");

            var infoText = LocalizationHelper.Localize("Welcome to Celestia. To learn more about using Celestia on Xbox, please visit here.");
            var infoLinkText = LocalizationHelper.Localize("here");
            var linkTextPos = infoText.IndexOf(infoLinkText);
            if (linkTextPos == -1)
            {
                WelcomeMessageContent.Visibility = Visibility.Collapsed;
            }
            else
            {
                var textBefore = infoText.Substring(0, linkTextPos);
                var textAfter = infoText.Substring(linkTextPos + infoLinkText.Length);
                var span = new Span();
                span.Inlines.Add(new Run() { Text = textBefore });

                var link = new Hyperlink();
                link.Inlines.Add(new Run() { Text = infoLinkText });
                link.NavigateUri = new Uri($"https://celestia.mobi/help/xbox?lang={LocalizationHelper.Locale}");
                span.Inlines.Add(link);

                span.Inlines.Add(new Run() { Text = textAfter });
                WelcomeMessageContent.Inlines.Add(span);
            }

            IgnoreCheckBox.Content = LocalizationHelper.Localize("Don't show this message again");
        }

        public bool ShouldNotShowMessageAgain
        {
            get
            {
                var isChecked = IgnoreCheckBox.IsChecked;
                return isChecked != null && (bool)isChecked;
            }
        }
    }
}
