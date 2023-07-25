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
using Microsoft.UI.Xaml.Controls;
using System;
using Windows.ApplicationModel.DataTransfer;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Web
{
    public sealed partial class CommonWebPage : Page
    {
        private JavascriptBridge bridge;
        private Uri initialUri;
        private string[] matchingQueryKeys;

        public CommonWebPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = (CommonWebArgs)e.Parameter;
            initialUri = parameter.Uri;
            matchingQueryKeys = parameter.MatchingQueryKeys;
            var contextDirectory = parameter.ContextDirectory;
            if (contextDirectory == null)
                contextDirectory = "";
            bridge = new JavascriptBridge(parameter.AppCore, parameter.Renderer, contextDirectory, parameter.ACKReceiver, (title, uri) =>
            {
                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    ShowShare(title, uri);
                });
            });
            EnsureWebView2();
        }

        private async void EnsureWebView2()
        {
            try
            {
                await WebView.EnsureCoreWebView2Async();
                WebView.Source = initialUri;
                WebView.CoreWebView2.DOMContentLoaded += CoreWebView2_DOMContentLoaded;
            } catch {}
        }

        private void CoreWebView2_DOMContentLoaded(Microsoft.Web.WebView2.Core.CoreWebView2 sender, Microsoft.Web.WebView2.Core.CoreWebView2DOMContentLoadedEventArgs args)
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

        private void WebView_NavigationStarting(WebView2 sender, Microsoft.Web.WebView2.Core.CoreWebView2NavigationStartingEventArgs args)
        {
            if (args.Uri == null)
            {
                args.Cancel = true;
                return;
            }
            var uri = new Uri(args.Uri);
            if (!isURIAllowed(uri))
            {
                args.Cancel = true;
                _ = Windows.System.Launcher.LaunchUriAsync(uri);
            }
        }

        private void WebView_WebMessageReceived(WebView2 sender, Microsoft.Web.WebView2.Core.CoreWebView2WebMessageReceivedEventArgs args)
        {
            try
            {
                var message = args.TryGetWebMessageAsString();
                if (bridge != null && message != null)
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
