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

        private static int leftMouseButton = 1;
        private static int middleMouseButton = 2;
        private static int rightMouseButton = 4;

        private Point? mLastLeftMousePosition = null;
        private Point? mLastRightMousePosition = null;

        private GLView mGLView;
        private String mCurrentPath;

        public MainPage()
        {
            mAppCore = new CelestiaAppCore();

            InitializeComponent();

            TextBlock loadingText = new TextBlock();
            loadingText.Foreground = new SolidColorBrush(Colors.White);
            loadingText.HorizontalAlignment = HorizontalAlignment.Center;
            loadingText.VerticalAlignment = VerticalAlignment.Center;
            loadingText.FontSize = 30;

            MainContainer.Children.Add(loadingText);

            mGLView = new GLView();
            mGLView.Prepare += (sender) =>
            {
                CelestiaAppCore.InitGL();
                string installedPath = Windows.ApplicationModel.Package.Current.InstalledPath;
                mCurrentPath = installedPath + "\\CelestiaResources";
                Directory.SetCurrentDirectory(mCurrentPath);
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
                    SetUpGLViewInteractions();
                    PopulateMenuBar();
                });

                mAppCore.Start();
                return true;
            };
            mGLView.Resize += (sender, width, height) =>
            {
                mAppCore.Resize(width, height);
            };
            mGLView.Draw += (sender) =>
            {
                mAppCore.Tick();
                mAppCore.Draw();
            };
            MainContainer.Children.Add(mGLView);
        }

        void SetUpGLViewInteractions()
        {
            mAppCore.SetContextMenuHandler((x, y, selection) =>
            {
                var menu = new MenuFlyout();
                var nameItem = new MenuFlyoutItem();
                nameItem.IsEnabled = false;
                nameItem.Text = mAppCore.Simulation.Universe.NameForSelection(selection);
                menu.Items.Add(nameItem);
                menu.Items.Add(new MenuFlyoutSeparator());
                var actions = new (String, short)[] {
                    ("Go", 103),
                    ("Follow", 102),
                    ("Orbit Synchronously", 121),
                    ("Lock Phase", 58),
                    ("Chase", 34),
                };
                foreach (var action in actions)
                {
                    var item = new MenuFlyoutItem();
                    item.Text = CelestiaAppCore.LocalizedString(action.Item1);
                    item.Click += (sender, arg) =>
                    {
                        mAppCore.Simulation.Selection = selection;
                        mAppCore.CharEnter(action.Item2);
                    };
                    menu.Items.Add(item);
                }
                
                menu.ShowAt(mGLView, new Point(x, y));
            });
            mGLView.PointerPressed += (sender, args) =>
            {
                if (args.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Mouse)
                {
                    var properties = args.GetCurrentPoint(this).Properties;
                    var position = args.GetCurrentPoint(this).Position;
                    if (properties.IsLeftButtonPressed)
                    {
                        mAppCore.MouseButtonDown((float)position.X, (float)position.Y, leftMouseButton);
                        mLastLeftMousePosition = position;
                    }
                    if (properties.IsRightButtonPressed)
                    {
                        mAppCore.MouseButtonDown((float)position.X, (float)position.Y, rightMouseButton);
                        mLastRightMousePosition = position;
                    }
                }
            };
            mGLView.PointerMoved += (sender, args) =>
            {
                if (args.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Mouse)
                {
                    var properties = args.GetCurrentPoint(this).Properties;
                    var position = args.GetCurrentPoint(this).Position;

                    if (properties.IsLeftButtonPressed && mLastLeftMousePosition != null)
                    {
                        var lastPos = mLastLeftMousePosition;
                        var oldPos = (Point)lastPos;

                        var x = position.X - oldPos.X;
                        var y = position.Y - oldPos.Y;
                        mAppCore.MouseMove((float)x, (float)y, leftMouseButton);
                        mLastLeftMousePosition = position;
                    }
                    if (properties.IsRightButtonPressed && mLastRightMousePosition != null)
                    {
                        var lastPos = mLastRightMousePosition;
                        var oldPos = (Point)lastPos;

                        var x = position.X - oldPos.X;
                        var y = position.Y - oldPos.Y;
                        mAppCore.MouseMove((float)x, (float)y, rightMouseButton);
                        mLastRightMousePosition = position;
                    }
                }
            };
            mGLView.PointerReleased += (sender, args) =>
            {
                if (args.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Mouse)
                {
                    var properties = args.GetCurrentPoint(this).Properties;
                    var position = args.GetCurrentPoint(this).Position;
                    if (mLastLeftMousePosition != null && !properties.IsLeftButtonPressed)
                    {
                        mAppCore.MouseButtonUp((float)position.X, (float)position.Y, leftMouseButton);
                        mLastLeftMousePosition = null;
                    }
                    if (mLastRightMousePosition != null && !properties.IsRightButtonPressed)
                    {
                        mAppCore.MouseButtonUp((float)position.X, (float)position.Y, rightMouseButton);
                        mLastRightMousePosition = null;
                    }
                }
            };
        }
        void PopulateMenuBar()
        {
            var fileItem = new MenuBarItem();
            fileItem.Title = CelestiaAppCore.LocalizedString("File");

            AppendItem(fileItem, CelestiaAppCore.LocalizedString("Open Script..."), (sender, arg) =>
            {
                PickScript();
            });

            var scriptsItem = new MenuFlyoutSubItem();
            scriptsItem.Text = CelestiaAppCore.LocalizedString("Scripts");
            var scripts = CelestiaAppCore.ReadScripts(mCurrentPath + "\\scripts", true);
            foreach (var script in scripts)
            {
                AppendSubItem(scriptsItem, script.Title, (sender, arg) =>
                {
                    mAppCore.RunScript(script.Filename);
                });
            }
            fileItem.Items.Add(scriptsItem);

            fileItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(fileItem, CelestiaAppCore.LocalizedString("Exit"), (sender, arg) =>
            {
                Application.Current.Exit();
            });

            var navigationItem = new MenuBarItem();
            navigationItem.Title = CelestiaAppCore.LocalizedString("Navigation");

            AppendItem(navigationItem, CelestiaAppCore.LocalizedString("Select Sol"), (sender, arg) =>
            {
                mAppCore.CharEnter(104);
            });
            AppendItem(navigationItem, CelestiaAppCore.LocalizedString("Tour Guide"), (sender, arg) =>
            {
                ShowTourGuide();
            });

            var timeItem = new MenuBarItem();
            timeItem.Title = CelestiaAppCore.LocalizedString("Time");
            var renderItem = new MenuBarItem();
            renderItem.Title = CelestiaAppCore.LocalizedString("Render");
            var viewItem = new MenuBarItem();
            viewItem.Title = CelestiaAppCore.LocalizedString("View");
            var bookmarkItem = new MenuBarItem();
            bookmarkItem.Title = CelestiaAppCore.LocalizedString("Bookmarks");
            var helpItem = new MenuBarItem();
            helpItem.Title = CelestiaAppCore.LocalizedString("Help");

            MenuBar.Items.Add(fileItem);
            MenuBar.Items.Add(navigationItem);
            MenuBar.Items.Add(timeItem);
            MenuBar.Items.Add(renderItem);
            MenuBar.Items.Add(viewItem);
            MenuBar.Items.Add(bookmarkItem);
            MenuBar.Items.Add(helpItem);
        }

        void AppendItem(MenuBarItem parent, String text, RoutedEventHandler click)
        {
            var item = new MenuFlyoutItem();
            item.Text = text;
            item.Click += click;
            parent.Items.Add(item);
        }

        void AppendSubItem(MenuFlyoutSubItem parent, String text, RoutedEventHandler click)
        {
            var item = new MenuFlyoutItem();
            item.Text = text;
            item.Click += click;
            parent.Items.Add(item);
        }

        async void PickScript()
        {
            var picker = new Windows.Storage.Pickers.FileOpenPicker();
            picker.ViewMode = Windows.Storage.Pickers.PickerViewMode.Thumbnail;
            picker.SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.Downloads;
            picker.FileTypeFilter.Add(".cel");
            picker.FileTypeFilter.Add(".celx");
            var file = await picker.PickSingleFileAsync();
            if (file != null)
                mAppCore.RunScript(file.Path);
        }
        void ShowTourGuide()
        {

        }
    }
}
