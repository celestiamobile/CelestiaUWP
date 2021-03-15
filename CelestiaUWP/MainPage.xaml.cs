using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

using GLUWP;
using CelestiaComponent;
using Windows.UI.Core;

namespace CelestiaUWP
{
    public sealed partial class MainPage : Page
    {
        private CelestiaAppCore mAppCore;

        public MainPage()
        {
            mAppCore = new CelestiaAppCore();

            InitializeComponent();

            TextBlock loadingText = new TextBlock();
            loadingText.Foreground = new SolidColorBrush(Colors.White);
            loadingText.HorizontalAlignment = HorizontalAlignment.Center;
            loadingText.VerticalAlignment = VerticalAlignment.Center;
            loadingText.FontSize = 30;

            grid.Children.Add(loadingText);

            GLView view = new GLView();
            view.Prepare += (sender) =>
            {
                CelestiaAppCore.InitGL();
                string installedPath = Windows.ApplicationModel.Package.Current.InstalledPath;
                Directory.SetCurrentDirectory(installedPath + "\\CelestiaResources");
                string[] extraPaths = { };
                if (!mAppCore.StartSimulation("celestia.cfg", extraPaths, delegate (string progress)
                {
                    _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        loadingText.Text = progress;
                    });
                }))
                {
                    return false;
                }
                if (!mAppCore.StartRenderer())
                    return false;

                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    loadingText.Visibility = Visibility.Collapsed;
                });

                mAppCore.Start();
                return true;
            };
            view.Resize += (sender, width, height) =>
            {
                mAppCore.Resize(width, height);
            };
            view.Draw += (sender) =>
            {
                mAppCore.Tick();
                mAppCore.Draw();
            };

            grid.Children.Add(view);
        }
    }
}
