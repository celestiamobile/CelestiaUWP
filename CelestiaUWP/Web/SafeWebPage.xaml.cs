//
// SafeWebPage.xaml.cs
//
// Copyright © 2022 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaAppComponent;
using Microsoft.Web.WebView2.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Web
{
    public sealed partial class SafeWebPage : Page
    {
        public SafeWebPage()
        {
            this.InitializeComponent();
            WebViewNotFoundView.Text = LocalizationHelper.Localize("WebView is not available.", "WebView component is missing or disabled");
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = (CommonWebArgs)e.Parameter;
            string webViewVersion = null;
            try
            {
                webViewVersion = CoreWebView2Environment.GetAvailableBrowserVersionString();
            }
            catch {}
            if (webViewVersion == null)
            {
                WebContent.Visibility = Windows.UI.Xaml.Visibility.Visible;
                WebContent.Navigate(typeof(LegacyWebPage), parameter);
            }
            else
            {
                WebContent.Visibility = Windows.UI.Xaml.Visibility.Visible;
                WebContent.Navigate(typeof(CommonWebPage), parameter);
            }
        }
    }
}
