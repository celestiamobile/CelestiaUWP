//
// CommonWebPage.xaml.cs
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
using Windows.ApplicationModel.DataTransfer;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Web
{
    public sealed partial class LegacyWebPage : Page
    {
        private JavascriptBridge bridge;
        private Uri initialUri;
        private string[] matchingQueryKeys;

        public LegacyWebPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = (CommonWebArgs)e.Parameter;
            var uriBuilder = new UriBuilder(parameter.Uri);
            if (uriBuilder.Query == null)
                uriBuilder.Query = ActualTheme == Windows.UI.Xaml.ElementTheme.Dark ? "theme=dark" : "theme=light";
            else
                uriBuilder.Query += "&" + (ActualTheme == Windows.UI.Xaml.ElementTheme.Dark ? "theme=dark" : "theme=light");
            initialUri = uriBuilder.Uri;
            matchingQueryKeys = parameter.MatchingQueryKeys;
            var contextDirectory = parameter.ContextDirectory;
            if (contextDirectory == null)
                contextDirectory = "";
            bridge = new JavascriptBridge(parameter.AppCore, parameter.Renderer, contextDirectory, parameter.ACKReceiver, (title, uri) =>
            {
                _ = Dispatcher.TryRunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    ShowShare(title, uri);
                });
            });
            WebView.Source = initialUri;
            WebView.DOMContentLoaded += WebView_DOMContentLoaded;
        }

        private void WebView_DOMContentLoaded(WebView sender, WebViewDOMContentLoadedEventArgs args)
        {
            LoadingIndicator.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
        }

        private bool isURIAllowed(Uri uri)
        {
            if (uri.Host != initialUri.Host || uri.AbsolutePath != initialUri.AbsolutePath)
            {
                return false;
            }
            var query = System.Web.HttpUtility.ParseQueryString(uri.Query);
            var initialQuery = System.Web.HttpUtility.ParseQueryString(initialUri.Query);
            foreach (var key in matchingQueryKeys)
            {
                if (query[key] != initialQuery[key])
                    return false;
            }

            return true;
        }

        private void WebView_NavigationStarting(WebView sender, WebViewNavigationStartingEventArgs args)
        {
            if (args.Uri == null)
            {
                args.Cancel = true;
                return;
            }
            var uri = args.Uri;
            if (!isURIAllowed(uri))
            {
                args.Cancel = true;
                _ = Windows.System.Launcher.LaunchUriAsync(uri);
            }
        }

        private void WebView_ScriptNotify(object sender, NotifyEventArgs args)
        {
            try
            {
                var value = args.Value;
                if (value != null && value is string message && bridge != null)
                {
                    bridge.ReceivedMessage(message);
                }
            }
            catch { }
        }

        private void ShowShare(string title, Uri uri)
        {
            var transferManager = DataTransferManager.GetForCurrentView();
            transferManager.DataRequested += (s, e) =>
            {
                e.Request.Data.Properties.Title = title;
                e.Request.Data.SetWebLink(uri);
            };
            DataTransferManager.ShowShareUI();
        }
    }
}
