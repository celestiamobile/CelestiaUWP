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

using CelestiaComponent;
using Windows.UI.Core;
using Windows.UI.WindowManagement;
using Windows.UI.Xaml.Hosting;
using System.ComponentModel;
using System.Runtime.InteropServices;

namespace CelestiaUWP
{
    public sealed partial class MainPage : Page, INotifyPropertyChanged
    {
        private CelestiaAppCore mAppCore;
        private CelestiaRenderer mRenderer;

        private static int leftMouseButton = 1;
        private static int middleMouseButton = 2;
        private static int rightMouseButton = 4;

        private Point? mLastLeftMousePosition = null;
        private Point? mLastRightMousePosition = null;

        private string mCurrentPath;

        private readonly string[] mMarkers = new string[]
        {
            "Diamond", "Triangle", "Filled Square", "Plus", "X", "Left Arrow", "Right Arrow", "Up Arrow", "Down Arrow",
            "Circle", "Disk", "Crosshair"
        };

        private string mLocalePath
        {
            get { return mCurrentPath + "\\locale"; }
        }

        private float scale = 1.0f;

        public MainPage()
        {
            mAppCore = new CelestiaAppCore();

            InitializeComponent();

            scale = ((int)Windows.Graphics.Display.DisplayInformation.GetForCurrentView().ResolutionScale) / 100.0f;

            string installedPath = Windows.ApplicationModel.Package.Current.InstalledPath;
            mCurrentPath = installedPath + "\\CelestiaResources";
            Directory.SetCurrentDirectory(mCurrentPath);

            Loaded += MainPage_Loaded;
        }

        private void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            mRenderer = new CelestiaRenderer(() => {
                var locale = GetLocale().Result;
                CelestiaAppCore.SetLocaleDirectory(mLocalePath, locale);

                CelestiaAppCore.InitGL();
                string[] extraPaths = { };
                if (!mAppCore.StartSimulation("celestia.cfg", extraPaths, delegate (string progress)
                {
                    _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        LoadingText.Text = progress;
                    });
                }))
                {
                    return false;
                }
                mAppCore.SetDPI((int)(96 * scale));
                if (!mAppCore.StartRenderer())
                    return false;

                var fontMap = new Dictionary<string, (string, int, string, int)>() {
                    { "ja", ("NotoSansCJK-Regular.ttc", 0, "NotoSansCJK-Bold.ttc", 0) },
                    { "ko", ("NotoSansCJK-Regular.ttc", 1, "NotoSansCJK-Bold.ttc", 1) },
                    { "zh_CN", ("NotoSansCJK-Regular.ttc", 2, "NotoSansCJK-Bold.ttc", 2) },
                    { "zh_TW", ("NotoSansCJK-Regular.ttc", 3, "NotoSansCJK-Bold.ttc", 3) },
                    { "ar", ("NotoSansArabic-Regular.ttf", 0, "NotoSansArabic-Bold.ttf", 0) },
                };
                var defaultFont = ("NotoSans-Regular.ttf", 0, "NotoSans-Bold.ttf", 0);
                var font = fontMap.GetValueOrDefault(locale, defaultFont);

                mAppCore.SetFont(mCurrentPath + "\\fonts\\" + font.Item1, font.Item2, 9);
                mAppCore.SetTitleFont(mCurrentPath + "\\fonts\\" + font.Item3, font.Item4, 15);
                mAppCore.SetRenderFont(mCurrentPath + "\\fonts\\" + font.Item1, font.Item2, 9, CelestiaFontStyle.normal);
                mAppCore.SetRenderFont(mCurrentPath + "\\fonts\\" + font.Item3, font.Item4, 15, CelestiaFontStyle.large);

                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    LoadingText.Visibility = Visibility.Collapsed;
                    SetUpGLViewInteractions();
                    PopulateMenuBar();
                    mRenderer.SetSize((int)GLView.Width, (int)GLView.Height);
                });

                mAppCore.Start();
                return true;
            });
            mRenderer.SetCorePointer(mAppCore.Pointer);
            mRenderer.SetSurface(GLView, scale);
            GLView.SizeChanged += (view, arg) =>
            {
                mRenderer.SetSize((int)arg.NewSize.Width, (int)arg.NewSize.Height);
            };
            mRenderer.Start();
        }

        void SetUpGLViewInteractions()
        {
            mAppCore.SetContextMenuHandler((x, y, selection) =>
            {
                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                  {
                      var menu = new MenuFlyout();
                      var nameItem = new MenuFlyoutItem();
                      nameItem.IsEnabled = false;
                      nameItem.Text = mAppCore.Simulation.Universe.NameForSelection(selection);
                      menu.Items.Add(nameItem);
                      menu.Items.Add(new MenuFlyoutSeparator());
                      var actions = new (String, short)[]
                      {
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

                      if (selection.Object is CelestiaBody)
                      {
                          var body = (CelestiaBody)selection.Object;
                          var surfaces = body.AlternateSurfaceNames;
                          if (surfaces != null && surfaces.Length > 0)
                          {
                              menu.Items.Add(new MenuFlyoutSeparator());

                              var altSur = new MenuFlyoutSubItem();
                              altSur.Text = CelestiaAppCore.LocalizedString("Alternate Surfaces");
                              AppendSubItem(altSur, CelestiaAppCore.LocalizedString("Default"), (sender, arg) =>
                              {
                                  mAppCore.Simulation.ActiveObserver.DisplayedSurfaceName = "";
                              });

                              foreach (var name in surfaces)
                              {
                                  AppendSubItem(altSur, name, (sender, arg) =>
                                  {
                                      mAppCore.Simulation.ActiveObserver.DisplayedSurfaceName = name;
                                  });
                              }

                              menu.Items.Add(altSur);
                          }
                      }

                      menu.Items.Add(new MenuFlyoutSeparator());

                      if (mAppCore.Simulation.Universe.IsSelectionMarked(selection))
                      {
                          var action = new MenuFlyoutItem();
                          action.Text = CelestiaAppCore.LocalizedString("Unmark");
                          action.Click += (sender, arg) =>
                          {
                              mAppCore.Simulation.Universe.UnmarkSelection(selection);
                          };
                          menu.Items.Add(action);
                      }
                      else
                      {
                          var action = new MenuFlyoutSubItem();
                          action.Text = CelestiaAppCore.LocalizedString("Mark");
                          for (int i = 0; i < mMarkers.Length; i += 1)
                          {
                              int copy = i;
                              var markerAction = new MenuFlyoutItem();
                              markerAction.Text = CelestiaAppCore.LocalizedString(mMarkers[i]);
                              markerAction.Click += (sender, arg) =>
                              {
                                  mAppCore.Simulation.Universe.MarkSelection(selection, (CelestiaMarkerRepresentation)copy);
                                  mAppCore.ShowMarkers = true;
                              };
                              action.Items.Add(markerAction);
                          }
                          menu.Items.Add(action);
                      }

                      menu.ShowAt(GLView, new Point(x / scale, y / scale));
                  });
            });
            GLView.PointerPressed += (sender, args) =>
            {
                if (args.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Mouse)
                {
                    var properties = args.GetCurrentPoint((UIElement)sender).Properties;
                    var position = args.GetCurrentPoint((UIElement)sender).Position;
                    position = new Point(position.X * scale, position.Y * scale);
                    if (properties.IsLeftButtonPressed)
                    {
                        mLastLeftMousePosition = position;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseButtonDown((float)position.X, (float)position.Y, leftMouseButton);
                        });
                    }
                    if (properties.IsRightButtonPressed)
                    {
                        mLastRightMousePosition = position;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseButtonDown((float)position.X, (float)position.Y, rightMouseButton);
                        });
                    }
                }
            };
            GLView.PointerMoved += (sender, args) =>
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
                        mLastLeftMousePosition = position;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseMove((float)x, (float)y, leftMouseButton);
                        });
                    }
                    if (properties.IsRightButtonPressed && mLastRightMousePosition != null)
                    {
                        var lastPos = mLastRightMousePosition;
                        var oldPos = (Point)lastPos;

                        var x = position.X - oldPos.X;
                        var y = position.Y - oldPos.Y;
                        mLastRightMousePosition = position;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseMove((float)x, (float)y, rightMouseButton);
                        });
                    }
                }
            };
            GLView.PointerReleased += (sender, args) =>
            {
                if (args.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Mouse)
                {
                    var properties = args.GetCurrentPoint((UIElement)sender).Properties;
                    var position = args.GetCurrentPoint((UIElement)sender).Position;
                    position = new Point(position.X * scale, position.Y * scale);
                    if (mLastLeftMousePosition != null && !properties.IsLeftButtonPressed)
                    {
                        mLastLeftMousePosition = null;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseButtonUp((float)position.X, (float)position.Y, leftMouseButton);
                        });
                    }
                    if (mLastRightMousePosition != null && !properties.IsRightButtonPressed)
                    {
                        mLastRightMousePosition = null;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseButtonUp((float)position.X, (float)position.Y, rightMouseButton);
                        });
                    }
                }
            };
            Window.Current.CoreWindow.CharacterReceived += (sender, arg) =>
            {
                mAppCore.CharEnter((short)arg.KeyCode);
            };
            Window.Current.CoreWindow.KeyDown += (sender, arg) =>
            {
                var modifiers = 0;
                if (CoreWindow.GetForCurrentThread().GetKeyState(Windows.System.VirtualKey.Control) == CoreVirtualKeyStates.Down)
                    modifiers |= 16;
                if (CoreWindow.GetForCurrentThread().GetKeyState(Windows.System.VirtualKey.Shift) == CoreVirtualKeyStates.Down)
                    modifiers |= 8;
                mAppCore.KeyDown((int)arg.VirtualKey, modifiers);
            };
            Window.Current.CoreWindow.KeyUp += (sender, arg) =>
            {
                mAppCore.KeyUp((int)arg.VirtualKey, 0);
            };
        }

        void PopulateMenuBar()
        {
            MenuBar.AllowFocusOnInteraction = false;
            MenuBar.IsFocusEngagementEnabled = false;
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

        void ShowViewOptions()
        {
            OverlayBackground.Visibility = Visibility.Visible;
            OverlayBackground.PointerPressed += (sender, arg) =>
            {
                OverlayBackground.Visibility = Visibility.Collapsed;
                OverlayContainer.Content = null;
            };
            OverlayContainer.Width = 580;
            OverlayContainer.Height = 670;
            OverlayContainer.Navigate(typeof(ViewOptionsPage), mAppCore);
        }

        void ShowLocationSettings()
        {
            OverlayBackground.Visibility = Visibility.Visible;
            OverlayBackground.PointerPressed += (sender, arg) =>
            {
                OverlayBackground.Visibility = Visibility.Collapsed;
                OverlayContainer.Content = null;
            };
            OverlayContainer.Width = 400;
            OverlayContainer.Height = 350;
            OverlayContainer.Navigate(typeof(LocationSettingsPage), mAppCore);
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
        async System.Threading.Tasks.Task<string> GetLocale()
        {
            var folder = await Windows.Storage.StorageFolder.GetFolderFromPathAsync(mLocalePath);
            var files = await folder.GetFoldersAsync();
            var availableLocales = new List<string>();
            var preferredLocale = System.Globalization.CultureInfo.CurrentCulture.Name;
            preferredLocale = preferredLocale.Replace("-", "_");
            foreach (var file in files)
            {
                availableLocales.Add(file.Name);
            }
            if (availableLocales.Contains(preferredLocale))
                return preferredLocale;
            var components = new List<string>(preferredLocale.Split("_"));
            if (components.Count() == 3)
                components.RemoveAt(1);
            if (components.Count() == 2)
                components[1] = components[1].ToUpper();
            preferredLocale = string.Join("_", components);
            if (availableLocales.Contains(preferredLocale))
                return preferredLocale;

            foreach (var lang in availableLocales)
            {
                if (lang == components[0] || lang.StartsWith(components[0] + "_"))
                    return lang;
            }

            return "";
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
