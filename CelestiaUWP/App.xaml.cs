using Microsoft.AppCenter;
using Microsoft.AppCenter.Analytics;
using Microsoft.AppCenter.Crashes;
using System;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    sealed partial class App : Application
    {
        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
            this.InitializeComponent();
            this.Suspending += OnSuspending;
            AppCenter.Start("06fcb18f-a477-4b2d-9cd9-f1b37135c16b", typeof(Analytics), typeof(Crashes));
        }

        protected override void OnLaunched(LaunchActivatedEventArgs e)
        {
            Launch(e, null, null);
        }

        protected override void OnFileActivated(FileActivatedEventArgs e)
        {
            Launch(null, e, null);
        }

        protected override void OnActivated(IActivatedEventArgs e)
        {
            Launch(null, null, e);
        }

        private void Launch(LaunchActivatedEventArgs launchEvent, FileActivatedEventArgs fileEvent, IActivatedEventArgs urlEvent)
        {
            Frame rootFrame = Window.Current.Content as Frame;

            // Do not repeat app initialization when the Window already has content,
            // just ensure that the window is active
            if (rootFrame == null)
            {
                // Create a Frame to act as the navigation context and navigate to the first page
                rootFrame = new Frame();

                rootFrame.NavigationFailed += OnNavigationFailed;
                // Place the frame in the current Window
                Window.Current.Content = rootFrame;
            }

            if (launchEvent == null || launchEvent.PrelaunchActivated == false)
            {
                if (rootFrame.Content == null)
                {
                    // When the navigation stack isn't restored navigate to the first page,
                    // configuring the new page by passing required information as a navigation
                    // parameter
                    if (launchEvent != null)
                        rootFrame.Navigate(typeof(MainPage), launchEvent.Arguments);
                    else
                        rootFrame.Navigate(typeof(MainPage));
                }

                if (fileEvent != null && fileEvent.Files != null && fileEvent.Files.Count > 0)
                {
                    var file = fileEvent.Files[0];
                    if (file is Windows.Storage.StorageFile)
                    {
                        var p = rootFrame.Content as MainPage;
                        p.OpenFileIfReady((Windows.Storage.StorageFile)file);
                    }
                }
                else if (urlEvent != null && urlEvent.Kind == ActivationKind.Protocol)
                {
                    var e = urlEvent as ProtocolActivatedEventArgs;
                    var p = rootFrame.Content as MainPage;
                    p.OpenURLIfReady(e.Uri);
                }

                // Ensure the current window is active
                Window.Current.Activate();
            }
        }

        /// <summary>
        /// Invoked when Navigation to a certain page fails
        /// </summary>
        /// <param name="sender">The Frame which failed navigation</param>
        /// <param name="e">Details about the navigation failure</param>
        void OnNavigationFailed(object sender, NavigationFailedEventArgs e)
        {
            throw new Exception("Failed to load Page " + e.SourcePageType.FullName);
        }

        /// <summary>
        /// Invoked when application execution is being suspended.  Application state is saved
        /// without knowing whether the application will be terminated or resumed with the contents
        /// of memory still intact.
        /// </summary>
        /// <param name="sender">The source of the suspend request.</param>
        /// <param name="e">Details about the suspend request.</param>
        private void OnSuspending(object sender, SuspendingEventArgs e)
        {
            var deferral = e.SuspendingOperation.GetDeferral();
            //TODO: Save application state and stop any background activity
            deferral.Complete();
        }
    }
}
