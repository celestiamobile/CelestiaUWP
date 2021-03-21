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
using Windows.UI.Xaml.Media;

using GLUWP;
using CelestiaComponent;
using Windows.UI.Core;
using Windows.UI.WindowManagement;
using Windows.UI.Xaml.Hosting;
using System.ComponentModel;

namespace CelestiaUWP
{
    public sealed partial class MainPage : Page, INotifyPropertyChanged
    {
        private CelestiaAppCore mAppCore;

        private static int leftMouseButton = 1;
        private static int middleMouseButton = 2;
        private static int rightMouseButton = 4;

        private Point? mLastLeftMousePosition = null;
        private Point? mLastRightMousePosition = null;

        private GLView mGLView;
        private String mCurrentPath;

        private float scale = 1.0f;

        public MainPage()
        {
            mAppCore = new CelestiaAppCore();

            InitializeComponent();

            TextBlock loadingText = new TextBlock();
            loadingText.Foreground = new SolidColorBrush(Colors.White);
            loadingText.HorizontalAlignment = HorizontalAlignment.Center;
            loadingText.VerticalAlignment = VerticalAlignment.Center;
            loadingText.FontSize = 30;

            scale = ((int)Windows.Graphics.Display.DisplayInformation.GetForCurrentView().ResolutionScale) / 100.0f;

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
                mAppCore.SetDPI((int)(96 * scale));
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
                    ("Track", 116)
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
                
                menu.ShowAt(mGLView, new Point(x / scale, y / scale));
            });
            mGLView.PointerPressed += (sender, args) =>
            {
                if (args.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Mouse)
                {
                    var properties = args.GetCurrentPoint((UIElement)sender).Properties;
                    var position = args.GetCurrentPoint((UIElement)sender).Position;
                    position = new Point(position.X * scale, position.Y * scale);
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
                    var properties = args.GetCurrentPoint((UIElement)sender).Properties;
                    var position = args.GetCurrentPoint((UIElement)sender).Position;
                    position = new Point(position.X * scale, position.Y * scale);
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
                    var properties = args.GetCurrentPoint((UIElement)sender).Properties;
                    var position = args.GetCurrentPoint((UIElement)sender).Position;
                    position = new Point(position.X * scale, position.Y * scale);
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
            MenuBar.Visibility = Visibility.Visible;
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
            AppendItem(navigationItem, CelestiaAppCore.LocalizedString("Select Object"), (sender, arg) =>
            {
                ShowSelectObject();
            });
            AppendItem(navigationItem, CelestiaAppCore.LocalizedString("Go to Object"), (sender, arg) =>
            {
                ShowGotoObject();
            });

            navigationItem.Items.Add(new MenuFlyoutSeparator());

            var actions = new (String, short)[] {
                    ("Go", 103),
                    ("Follow", 102),
                    ("Orbit Synchronously", 121),
                    ("Lock Phase", 58),
                    ("Chase", 34),
                    ("Track", 116)
                };
            foreach (var action in actions)
            {
                AppendItem(navigationItem, CelestiaAppCore.LocalizedString(action.Item1), (sender, arg) =>
                {
                    mAppCore.CharEnter(action.Item2);
                });
            }
            navigationItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(navigationItem, CelestiaAppCore.LocalizedString("Browser"), (sender, arg) =>
            {
                ShowBrowser();
            });
            AppendItem(navigationItem, CelestiaAppCore.LocalizedString("Eclipse Finder"), (sender, arg) =>
            {
                ShowEclipseFinder();
            });

            var timeItem = new MenuBarItem();
            timeItem.Title = CelestiaAppCore.LocalizedString("Time");
            AppendItem(timeItem, CelestiaAppCore.LocalizedString("10x Faster"), (sender, arg) =>
            {
                mAppCore.CharEnter(108);
            });
            AppendItem(timeItem, CelestiaAppCore.LocalizedString("10x Slower"), (sender, arg) =>
            {
                mAppCore.CharEnter(107);
            });
            AppendItem(timeItem, CelestiaAppCore.LocalizedString("Freeze"), (sender, arg) =>
            {
                mAppCore.CharEnter(32);
            });
            AppendItem(timeItem, CelestiaAppCore.LocalizedString("Real Time"), (sender, arg) =>
            {
                mAppCore.CharEnter(33);
            });
            AppendItem(timeItem, CelestiaAppCore.LocalizedString("Reverse Time"), (sender, arg) =>
            {
                mAppCore.CharEnter(106);
            });

            timeItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(timeItem, CelestiaAppCore.LocalizedString("Set Time..."), (sender, arg) =>
            {
                ShowTimeSetting();
            });


            var renderItem = new MenuBarItem();
            renderItem.Title = CelestiaAppCore.LocalizedString("Render");

            AppendItem(renderItem, CelestiaAppCore.LocalizedString("View Options"), (sender, arg) =>
            {
                ShowViewOptions();
            });
            AppendItem(renderItem, CelestiaAppCore.LocalizedString("Locations"), (sender, arg) =>
            {
                ShowLocationSettings();
            });
            renderItem.Items.Add(new MenuFlyoutSeparator());
            AppendItem(renderItem, CelestiaAppCore.LocalizedString("More Stars Visible"), (sender, arg) =>
            {
                mAppCore.CharEnter(93);
            });
            AppendItem(renderItem, CelestiaAppCore.LocalizedString("Fewer Stars Visible"), (sender, arg) =>
            {
                mAppCore.CharEnter(91);
            });
            var starStyleItem = new MenuFlyoutSubItem();
            starStyleItem.Text = CelestiaAppCore.LocalizedString("Star Style");
            AppendSubItem(starStyleItem, CelestiaAppCore.LocalizedString("Fuzzy Points"), (sender, arg) =>
            {
                mAppCore.StarStyle = 0;
            });
            AppendSubItem(starStyleItem, CelestiaAppCore.LocalizedString("Points"), (sender, arg) =>
            {
                mAppCore.StarStyle = 1;
            });
            AppendSubItem(starStyleItem, CelestiaAppCore.LocalizedString("Scaled Discs"), (sender, arg) =>
            {
                mAppCore.StarStyle = 2;
            });
            renderItem.Items.Add(starStyleItem);
            renderItem.Items.Add(new MenuFlyoutSeparator());
            var resolutionItem = new MenuFlyoutSubItem();
            resolutionItem.Text = CelestiaAppCore.LocalizedString("Texture Resolution");
            AppendSubItem(resolutionItem, CelestiaAppCore.LocalizedString("Low"), (sender, arg) =>
            {
                mAppCore.Resolution = 0;
            });
            AppendSubItem(resolutionItem, CelestiaAppCore.LocalizedString("Medium"), (sender, arg) =>
            {
                mAppCore.Resolution = 1;
            });
            AppendSubItem(resolutionItem, CelestiaAppCore.LocalizedString("High"), (sender, arg) =>
            {
                mAppCore.Resolution = 2;
            });
            renderItem.Items.Add(resolutionItem);

            var viewItem = new MenuBarItem();
            viewItem.Title = CelestiaAppCore.LocalizedString("View");
            AppendItem(viewItem, CelestiaAppCore.LocalizedString("Split Horizontally"), (sender, arg) =>
            {
                mAppCore.CharEnter(18);
            });
            AppendItem(viewItem, CelestiaAppCore.LocalizedString("Split Vertically"), (sender, arg) =>
            {
                mAppCore.CharEnter(21);
            });
            AppendItem(viewItem, CelestiaAppCore.LocalizedString("Delete Active View"), (sender, arg) =>
            {
                mAppCore.CharEnter(127);
            });
            AppendItem(viewItem, CelestiaAppCore.LocalizedString("Single View"), (sender, arg) =>
            {
                mAppCore.CharEnter(4);
            });

            var bookmarkItem = new MenuBarItem();
            bookmarkItem.Title = CelestiaAppCore.LocalizedString("Bookmarks");
            AppendItem(bookmarkItem, CelestiaAppCore.LocalizedString("Add Bookmarks"), (sender, arg) =>
            {
                ShowNewBookmark();
            });
            AppendItem(bookmarkItem, CelestiaAppCore.LocalizedString("Organize Bookmarks"), (sender, arg) =>
            {
                ShowBookmarkOrganizer();
            });

            var helpItem = new MenuBarItem();
            helpItem.Title = CelestiaAppCore.LocalizedString("Help");
            AppendItem(helpItem, CelestiaAppCore.LocalizedString("Run Demo"), (sender, arg) =>
            {
                mAppCore.CharEnter(100);
            });
            helpItem.Items.Add(new MenuFlyoutSeparator());
            AppendItem(helpItem, CelestiaAppCore.LocalizedString("OpenGL Info"), (sender, arg) =>
            {
                ShowOpenGLInfo();
            });

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
        async void ShowTourGuide()
        {
            AppWindow appWindow = await AppWindow.TryCreateAsync();
            Frame appWindowContentFrame = new Frame();
            appWindowContentFrame.Navigate(typeof(TourGuidePage), mAppCore);
            ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowContentFrame);
            await appWindow.TryShowAsync();
            appWindow.Closed += delegate
            {
                appWindowContentFrame.Content = null;
                appWindow = null;
            };
        }
        async void ShowSelectObject()
        {
            var dialog = new TextInputDialog(CelestiaAppCore.LocalizedString("Object name:"));
            var result = await dialog.ShowAsync();
            if (result == ContentDialogResult.Primary)
            {
                var text = dialog.Text;
                var selection = mAppCore.Simulation.Find(text);
                if (selection.IsEmpty)
                {
                    var alert = new ContentDialog();
                    alert.Title = CelestiaAppCore.LocalizedString("Object not found.");
                    alert.PrimaryButtonText = CelestiaAppCore.LocalizedString("OK");
                    await alert.ShowAsync();
                }
                else
                {
                    mAppCore.Simulation.Selection = selection;
                }
            }
        }
        async void ShowGotoObject()
        {
            var dialog = new GotoObjectDialog();
            var result = await dialog.ShowAsync();
            if (result == ContentDialogResult.Primary)
            {
                var objectName = dialog.Text;
                var latitude = dialog.Latitude;
                var longitude = dialog.Longitude;
                var distance = dialog.Distance;
                var unit = (CelestiaGotoLocationDistanceUnit)dialog.Unit;
                var selection = mAppCore.Simulation.Find(objectName);
                if (selection.IsEmpty)
                {
                    var alert = new ContentDialog();
                    alert.Title = CelestiaAppCore.LocalizedString("Object not found.");
                    alert.PrimaryButtonText = CelestiaAppCore.LocalizedString("OK");
                    await alert.ShowAsync();
                }
                else
                {
                    var location = new CelestiaGotoLocation(selection, latitude, longitude, distance, unit);
                    mAppCore.Simulation.GoToLocation(location);
                }
            }
        }

        async void ShowBrowser()
        {
            AppWindow appWindow = await AppWindow.TryCreateAsync();
            Frame appWindowContentFrame = new Frame();
            appWindowContentFrame.Navigate(typeof(BrowserPage), mAppCore);
            ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowContentFrame);
            await appWindow.TryShowAsync();
            appWindow.Closed += delegate
            {
                appWindowContentFrame.Content = null;
                appWindow = null;
            };
        }

        async void ShowEclipseFinder()
        {
            AppWindow appWindow = await AppWindow.TryCreateAsync();
            Frame appWindowContentFrame = new Frame();
            appWindowContentFrame.Navigate(typeof(EclipseFinderPage), mAppCore);
            ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowContentFrame);
            await appWindow.TryShowAsync();
            appWindow.Closed += delegate
            {
                appWindowContentFrame.Content = null;
                appWindow = null;
            };
        }

        async void ShowTimeSetting()
        {
            var dialog = new TimeSettingDialog(mAppCore.Simulation.Time);
            var result = await dialog.ShowAsync();
            if (result == ContentDialogResult.Primary)
            {
                var date = dialog.DisplayDate;
                mAppCore.Simulation.Time = date;
            }
        }

        async void ShowViewOptions()
        {
            AppWindow appWindow = await AppWindow.TryCreateAsync();
            Frame appWindowContentFrame = new Frame();
            appWindowContentFrame.Navigate(typeof(ViewOptionsPage), mAppCore);
            ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowContentFrame);
            await appWindow.TryShowAsync();
            appWindow.Closed += delegate
            {
                appWindowContentFrame.Content = null;
                appWindow = null;
            };
        }

        async void ShowLocationSettings()
        {
            AppWindow appWindow = await AppWindow.TryCreateAsync();
            Frame appWindowContentFrame = new Frame();
            appWindowContentFrame.Navigate(typeof(LocationSettingsPage), mAppCore);
            ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowContentFrame);
            await appWindow.TryShowAsync();
            appWindow.Closed += delegate
            {
                appWindowContentFrame.Content = null;
                appWindow = null;
            };
        }

        async void ShowBookmarkOrganizer()
        {
            AppWindow appWindow = await AppWindow.TryCreateAsync();
            Frame appWindowContentFrame = new Frame();
            appWindowContentFrame.Navigate(typeof(BookmarkOrganizerPage), mAppCore);
            ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowContentFrame);
            await appWindow.TryShowAsync();
            appWindow.Closed += delegate
            {
                appWindowContentFrame.Content = null;
                appWindow = null;
            };
        }

        async void ShowNewBookmark()
        {
            AppWindow appWindow = await AppWindow.TryCreateAsync();
            Frame appWindowContentFrame = new Frame();
            appWindowContentFrame.Navigate(typeof(NewBookmarkPage), mAppCore);
            ElementCompositionPreview.SetAppWindowContent(appWindow, appWindowContentFrame);
            await appWindow.TryShowAsync();
            appWindow.Closed += delegate
            {
                appWindowContentFrame.Content = null;
                appWindow = null;
            };
        }
        async void ShowOpenGLInfo()
        {
            var dialog = new InfoDialog(mAppCore.RenderInfo);
            dialog.Title = CelestiaAppCore.LocalizedString("OpenGL Info");
            await dialog.ShowAsync();
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
