//
// MainPage.xaml.cs
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
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.ApplicationModel.DataTransfer;
using Windows.Foundation;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace CelestiaUWP
{
    public sealed partial class MainPage : Page
    {
        private readonly CelestiaAppCore mAppCore;
        private CelestiaRenderer mRenderer;

        private Point? mLastLeftMousePosition = null;
        private Point? mLastRightMousePosition = null;
        private Point? mLastMiddleMousePosition = null;

        private string mExtraAddonFolder;
        private string mExtraScriptFolder;

        private Dictionary<string, object> mSettings;

        private Windows.Storage.StorageFile ScriptFileToOpen;
        private Uri URLToOpen;
        private bool ReadyForInput = false;

        private readonly AppSettings AppSettings = AppSettings.Shared;

        private readonly string[] Markers = new string[]
        {
            "Diamond", "Triangle", "Filled Square", "Plus", "X", "Left Arrow", "Right Arrow", "Up Arrow", "Down Arrow",
            "Circle", "Disk", "Crosshair"
        };

        private float scale = 1.0f;

        public MainPage()
        {
            mAppCore = new CelestiaAppCore();

            InitializeComponent();

            Loaded += MainPage_Loaded;
            SizeChanged += MainPage_SizeChanged;

            var coreTitleBar = CoreApplication.GetCurrentView().TitleBar;
            coreTitleBar.ExtendViewIntoTitleBar = false;
        }

        private void MainPage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (!ReadyForInput) return;

            var isFullScreen = ApplicationView.GetForCurrentView().IsFullScreenMode;
            mAppCore.SetSafeAreaInsets(0, isFullScreen ? 0 : (int)(MenuBar.Height * scale), 0, 0);
            MenuBar.Visibility = isFullScreen ? Visibility.Collapsed : Visibility.Visible;
        }

        private async void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            scale = AppSettings.UseFullDPI ? ((int)Windows.Graphics.Display.DisplayInformation.GetForCurrentView().ResolutionScale) / 100.0f : 1.0f;

            string installedPath = Windows.ApplicationModel.Package.Current.InstalledPath;

            Windows.Storage.StorageFolder customDataFolder = null;
            Windows.Storage.StorageFile customConfigFile = null;
            try
            {
                customDataFolder = await Windows.Storage.ApplicationData.Current.LocalFolder.GetFolderAsync("Override");
                customConfigFile = await customDataFolder.GetFileAsync("celestia.cfg");
            }
            catch { }

            var defaultResourcePath = installedPath + "\\CelestiaResources";
            var defaultConfigFilePath = defaultResourcePath + "\\celestia.cfg";

            var resourcePath = customDataFolder != null ? customDataFolder.Path : defaultResourcePath;
            var configPath = customConfigFile != null ? customConfigFile.Path : defaultConfigFilePath;

            var localePath = defaultResourcePath + "\\locale";
            var locale = await GetLocale(localePath);

            await Task.Run(() => CreateExtraFolders());

            mRenderer = new CelestiaRenderer(AppSettings.EnableMSAA, () => {
                CelestiaAppCore.InitGL();

                List<string> extraPaths = new List<string>();
                if (mExtraAddonFolder != null)
                    extraPaths.Add(mExtraAddonFolder);

                void progressCallback(string progress)
                {
                    _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        LoadingText.Text = string.Format(LocalizationHelper.Localize("Loading: %s").Replace("%s", "{0}"), progress);
                    });
                }

                Directory.SetCurrentDirectory(resourcePath);
                CelestiaAppCore.SetLocaleDirectory(resourcePath + "\\locale", locale);
                if (!mAppCore.StartSimulation(configPath, extraPaths.ToArray(), progressCallback) && (resourcePath != defaultResourcePath || configPath != defaultConfigFilePath))
                {
                    if (resourcePath != defaultResourcePath || configPath != defaultConfigFilePath)
                    {
                        // Try to restore originial settings
                        _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                        {
                            ContentDialogHelper.ShowAlert(this, LocalizationHelper.Localize("Error loading data, fallback to original configuration."));
                        });
                        Directory.SetCurrentDirectory(defaultResourcePath);
                        CelestiaAppCore.SetLocaleDirectory(defaultResourcePath + "\\locale", locale);
                        if (!mAppCore.StartSimulation(defaultConfigFilePath, extraPaths.ToArray(), progressCallback))
                        {
                            _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                            {
                                ShowLoadingFailure();
                            });
                            return false;
                        }
                    }
                    else
                    {
                        _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                        {
                            ShowLoadingFailure();
                        });
                        return false;
                    }
                }

                mAppCore.SetDPI((int)(96 * scale));
                if (!mAppCore.StartRenderer())
                {
                    _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        ShowLoadingFailure();
                    });
                    return false;
                }

                LocalizationHelper.Locale = CelestiaAppCore.LocalizedString("LANGUAGE", "celestia");
                var fontMap = new Dictionary<string, (string, int, string, int)>() {
                    { "ja", ("NotoSansCJK-Regular.ttc", 0, "NotoSansCJK-Bold.ttc", 0) },
                    { "ko", ("NotoSansCJK-Regular.ttc", 1, "NotoSansCJK-Bold.ttc", 1) },
                    { "zh_CN", ("NotoSansCJK-Regular.ttc", 2, "NotoSansCJK-Bold.ttc", 2) },
                    { "zh_TW", ("NotoSansCJK-Regular.ttc", 3, "NotoSansCJK-Bold.ttc", 3) },
                    { "ar", ("NotoSansArabic-Regular.ttf", 0, "NotoSansArabic-Bold.ttf", 0) },
                };
                var defaultFont = ("NotoSans-Regular.ttf", 0, "NotoSans-Bold.ttf", 0);
                var font = fontMap.GetValueOrDefault(LocalizationHelper.Locale, defaultFont);

                var pathPrefix = defaultResourcePath + "\\fonts\\";

                mAppCore.SetFont(pathPrefix + font.Item1, font.Item2, 9);
                mAppCore.SetTitleFont(pathPrefix + font.Item3, font.Item4, 15);
                mAppCore.SetRenderFont(pathPrefix + font.Item1, font.Item2, 9, CelestiaFontStyle.normal);
                mAppCore.SetRenderFont(pathPrefix + font.Item3, font.Item4, 15, CelestiaFontStyle.large);

                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    LoadingText.Visibility = Visibility.Collapsed;
                    SetUpGLViewInteractions();
                    PopulateMenuBar(resourcePath);
                    mRenderer.SetSize((int)GLView.ActualWidth, (int)GLView.ActualHeight);
                });

                mSettings = ReadSettings().Result;
                ApplySettings(mSettings);

                mAppCore.Start();

                ReadyForInput = true;

                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    OpenFileOrURL();
                });

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

        private void ShowLoadingFailure()
        {
            LoadingText.Text = LocalizationHelper.Localize("Loading Celestia failed…");
        }

        private async void OpenFileOrURL()
        {
            var scriptFile = ScriptFileToOpen;
            var url = URLToOpen;
            if (scriptFile != null)
            {
                ScriptFileToOpen = null;
                var fileExtension = scriptFile.FileType;
                var tempFolder = Windows.Storage.ApplicationData.Current.TemporaryFolder;
                try
                {
                    var copiedFile = await scriptFile.CopyAsync(tempFolder, GuidHelper.CreateNewGuid().ToString() + fileExtension, Windows.Storage.NameCollisionOption.ReplaceExisting);
                    if (await ContentDialogHelper.ShowOption(this, LocalizationHelper.Localize("Run script?")))
                    {
                        mAppCore.RunScript(copiedFile.Path);
                    }
                }
                catch { }
            }
            if (url != null)
            {
                URLToOpen = null;
                if (await ContentDialogHelper.ShowOption(this, LocalizationHelper.Localize("Open URL?")))
                {
                    mAppCore.GoToURL(url.AbsoluteUri);
                }
            }
        }

        public void OpenFileIfReady(Windows.Storage.StorageFile scriptFileToOpen)
        {
            ScriptFileToOpen = scriptFileToOpen;
            if (ReadyForInput)
                OpenFileOrURL();
        }

        public void OpenURLIfReady(Uri URL)
        {
            URLToOpen = URL;
            if (ReadyForInput)
                OpenFileOrURL();
        }

        private async void CreateExtraFolders()
        {
            var folder = Windows.Storage.ApplicationData.Current.LocalFolder;
            try
            {
                var mainFolder = await folder.CreateFolderAsync("CelestiaResources", Windows.Storage.CreationCollisionOption.OpenIfExists);
                var addonFolder = await mainFolder.CreateFolderAsync("extras", Windows.Storage.CreationCollisionOption.OpenIfExists);
                mExtraAddonFolder = addonFolder.Path;
                var scriptFolder = await mainFolder.CreateFolderAsync("scripts", Windows.Storage.CreationCollisionOption.OpenIfExists);
                mExtraScriptFolder = scriptFolder.Path;
            } catch { }
        }

        void SetUpGLViewInteractions()
        {
            Window.Current.VisibilityChanged += (sender, args) =>
            {
                AppSettings.Save();
                SaveSettings(GetCurrentSettings());
            };

            mAppCore.SetContextMenuHandler((x, y, selection) =>
            {
                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                  {
                      var menu = new MenuFlyout();
                      AppendItem(menu, mAppCore.Simulation.Universe.NameForSelection(selection), null);
                      menu.Items.Add(new MenuFlyoutSeparator());
                      var actions = new (string, short)[]
                      {
                        ("Go", 103),
                        ("Follow", 102),
                        ("Sync Orbit", 121),
                      };

                      foreach (var action in actions)
                      {
                          AppendItem(menu, LocalizationHelper.Localize(action.Item1), (sender, arg) =>
                          {
                              mAppCore.Simulation.Selection = selection;
                              mAppCore.CharEnter(action.Item2);
                          });
                      }

                      if (selection.Object is CelestiaBody body)
                      {
                          var surfaces = body.AlternateSurfaceNames;
                          if (surfaces != null && surfaces.Length > 0)
                          {
                              menu.Items.Add(new MenuFlyoutSeparator());

                              var altSur = new MenuFlyoutSubItem
                              {
                                  Text = LocalizationHelper.Localize("Alternate Surfaces")
                              };
                              AppendSubItem(altSur, LocalizationHelper.Localize("Default"), (sender, arg) =>
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

                          menu.Items.Add(new MenuFlyoutSeparator());
                          var refMarkMenu = new MenuFlyoutSubItem
                          {
                              Text = LocalizationHelper.Localize("Reference Vectors")
                          };

                          var refMarkMenuActions = new (string, string)[]
                          {
                              ("Show Body Axes", "ShowBodyAxes"),
                              ("Show Frame Axes", "ShowFrameAxes"),
                              ("Show Sun Direction", "ShowSunDirection"),
                              ("Show Velocity Vector", "ShowVelocityVector"),
                              ("Show Planetographic Grid", "ShowPlanetographicGrid"),
                              ("Show Terminator", "ShowTerminator")
                          };

                          foreach (var action in refMarkMenuActions)
                          {
                              var isEnabled = (bool)mAppCore.GetType().GetProperty(action.Item2).GetValue(mAppCore);
                              AppendToggleSubItem(refMarkMenu, LocalizationHelper.Localize(action.Item1), isEnabled, (sender, args) =>
                              {
                                  mAppCore.GetType().GetProperty(action.Item2).SetValue(mAppCore, !isEnabled);
                              });
                          }
                          menu.Items.Add(refMarkMenu);
                      }

                      var browserMenuItems = new List<MenuFlyoutItemBase>();
                      var browserItem = new CelestiaBrowserItem(mAppCore.Simulation.Universe.NameForSelection(selection), selection.Object, GetChildren);
                      if (browserItem.Children != null)
                      {
                          foreach (var child in browserItem.Children)
                          {
                              browserMenuItems.Add(CreateMenuItem(child));
                          }
                      }

                      if (browserMenuItems.Count > 0)
                      {
                          menu.Items.Add(new MenuFlyoutSeparator());
                          foreach (var browserMenuItem in browserMenuItems)
                          {
                              menu.Items.Add(browserMenuItem);
                          }
                      }

                      var url = CelestiaAppHelper.GetInfoURL(selection);
                      if (!string.IsNullOrEmpty(url))
                      {
                          menu.Items.Add(new MenuFlyoutSeparator());
                          AppendItem(menu, LocalizationHelper.Localize("Web Info"), (sender, arg) =>
                          {
                              _ = Launcher.LaunchUriAsync(new Uri(url));
                          });
                      }

                      menu.Items.Add(new MenuFlyoutSeparator());

                      if (mAppCore.Simulation.Universe.IsSelectionMarked(selection))
                      {
                          AppendItem(menu, LocalizationHelper.Localize("Unmark"), (sender, arg) =>
                          {
                              mAppCore.Simulation.Universe.UnmarkSelection(selection);
                          });
                      }
                      else
                      {
                          var action = new MenuFlyoutSubItem
                          {
                              Text = LocalizationHelper.Localize("Mark")
                          };
                          for (int i = 0; i < Markers.Length; i += 1)
                          {
                              int copy = i;
                              AppendSubItem(action, LocalizationHelper.Localize(Markers[i]), (sender, arg) =>
                              {
                                  mAppCore.Simulation.Universe.MarkSelection(selection, (CelestiaMarkerRepresentation)copy);
                                  mAppCore.ShowMarkers = true;
                              });
                          }
                          menu.Items.Add(action);
                      }

                      menu.ShowAt(GLView, new Point(x / scale, y / scale));
                  });
            });
            GLView.PointerPressed += (sender, args) =>
            {
                if (OverlayContainer.Content != null) return;
                if (VisualTreeHelper.GetOpenPopups(Window.Current).Count > 0) return;

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
                            mAppCore.MouseButtonDown((float)position.X, (float)position.Y, CelestiaMouseButton.left);
                        });
                    }
                    if (properties.IsRightButtonPressed)
                    {
                        mLastRightMousePosition = position;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseButtonDown((float)position.X, (float)position.Y, CelestiaMouseButton.right);
                        });
                    }
                    if (properties.IsMiddleButtonPressed)
                    {
                        mLastMiddleMousePosition = position;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseButtonDown((float)position.X, (float)position.Y, CelestiaMouseButton.middle);
                        });
                    }
                }
            };
            GLView.PointerMoved += (sender, args) =>
            {
                if (OverlayContainer.Content != null) return;
                if (VisualTreeHelper.GetOpenPopups(Window.Current).Count > 0) return;

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
                            mAppCore.MouseMove((float)x, (float)y, CelestiaMouseButton.left);
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
                            mAppCore.MouseMove((float)x, (float)y, CelestiaMouseButton.right);
                        });
                    }
                    if (properties.IsMiddleButtonPressed && mLastMiddleMousePosition != null)
                    {
                        var lastPos = mLastMiddleMousePosition;
                        var oldPos = (Point)lastPos;

                        var x = position.X - oldPos.X;
                        var y = position.Y - oldPos.Y;
                        mLastMiddleMousePosition = position;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseMove((float)x, (float)y, CelestiaMouseButton.middle);
                        });
                    }
                }
            };
            GLView.PointerReleased += (sender, args) =>
            {
                if (OverlayContainer.Content != null) return;
                if (VisualTreeHelper.GetOpenPopups(Window.Current).Count > 0) return;

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
                            mAppCore.MouseButtonUp((float)position.X, (float)position.Y, CelestiaMouseButton.left);
                        });
                    }
                    if (mLastRightMousePosition != null && !properties.IsRightButtonPressed)
                    {
                        mLastRightMousePosition = null;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseButtonUp((float)position.X, (float)position.Y, CelestiaMouseButton.right);
                        });
                    }
                    if (mLastMiddleMousePosition != null && !properties.IsMiddleButtonPressed)
                    {
                        mLastMiddleMousePosition = null;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseButtonUp((float)position.X, (float)position.Y, CelestiaMouseButton.middle);
                        });
                    }
                }
            };
            GLView.PointerWheelChanged += (sender, arg) =>
            {
                if (OverlayContainer.Content != null) return;
                if (VisualTreeHelper.GetOpenPopups(Window.Current).Count > 0) return;

                var delta = arg.GetCurrentPoint((UIElement)sender).Properties.MouseWheelDelta;
                mRenderer.EnqueueTask(() =>
                {
                    mAppCore.MouseWheel(delta > 0 ? 1 : -1, 0);
                });
            };
            Window.Current.CoreWindow.CharacterReceived += (sender, arg) =>
            {
                if (OverlayContainer.Content != null) return;
                if (VisualTreeHelper.GetOpenPopups(Window.Current).Count > 0) return;

                mRenderer.EnqueueTask(() =>
                {
                    mAppCore.CharEnter((short)arg.KeyCode);
                });
            };
            Window.Current.CoreWindow.KeyDown += (sender, arg) =>
            {
                if (OverlayContainer.Content != null) return;
                if (VisualTreeHelper.GetOpenPopups(Window.Current).Count > 0) return;


                var modifiers = 0;
                if (CoreWindow.GetForCurrentThread().GetKeyState(Windows.System.VirtualKey.Control) == CoreVirtualKeyStates.Down)
                    modifiers |= 16;
                if (CoreWindow.GetForCurrentThread().GetKeyState(Windows.System.VirtualKey.Shift) == CoreVirtualKeyStates.Down)
                    modifiers |= 8;

                mRenderer.EnqueueTask(() =>
                {
                    mAppCore.KeyDown((int)arg.VirtualKey, modifiers);
                });
            };
            Window.Current.CoreWindow.KeyUp += (sender, arg) =>
            {
                if (OverlayContainer.Content != null) return;
                if (VisualTreeHelper.GetOpenPopups(Window.Current).Count > 0) return;

                mRenderer.EnqueueTask(() =>
                {
                    mAppCore.KeyUp((int)arg.VirtualKey, 0);
                });
            };
        }

        void PopulateMenuBar(string resourcePath)
        {
            MenuBarItem CreateMenuBarItem(string name)
            {
                return new MenuBarItem
                {
                    Title = name,
                    IsTabStop = false,
                    AllowFocusOnInteraction = false
                };
            }

            var isFullScreen = ApplicationView.GetForCurrentView().IsFullScreenMode;
            mAppCore.SetSafeAreaInsets(0, isFullScreen ? 0 : (int)(MenuBar.Height * scale), 0, 0);
            MenuBar.Visibility = isFullScreen ? Visibility.Collapsed : Visibility.Visible;

            var fileItem = CreateMenuBarItem(LocalizationHelper.Localize("File"));

            AppendItem(fileItem, LocalizationHelper.Localize("Run Script…"), (sender, arg) =>
            {
                PickScript();
            });

            var scriptsItem = new MenuFlyoutSubItem
            {
                Text = LocalizationHelper.Localize("Scripts")
            };
            var scripts = CelestiaAppCore.ReadScripts(resourcePath + "\\scripts", true);
            if (scripts != null)
            {
                foreach (var script in scripts)
                {
                    AppendSubItem(scriptsItem, script.Title, (sender, arg) =>
                    {
                        mAppCore.RunScript(script.Filename);
                    });
                }
            }
            if (mExtraScriptFolder != null)
            {
                var extraScripts = CelestiaAppCore.ReadScripts(mExtraScriptFolder, true);
                if (extraScripts != null)
                {
                    foreach (var script in extraScripts)
                    {
                        AppendSubItem(scriptsItem, script.Title, (sender, arg) =>
                        {
                            mAppCore.RunScript(script.Filename);
                        });
                    }
                }
            }
            fileItem.Items.Add(scriptsItem);

            fileItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(fileItem, LocalizationHelper.Localize("Capture Image"), (sender, arg) =>
            {
                CaptureImage();
            }, new KeyboardAccelerator() { Key = Windows.System.VirtualKey.F10 });

            fileItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(fileItem, LocalizationHelper.Localize("Copy URL"), (sender, arg) =>
            {
                DataPackage dataPackage = new DataPackage
                {
                    RequestedOperation = DataPackageOperation.Copy
                };
                dataPackage.SetText(mAppCore.CurrentURL);
                Clipboard.SetContent(dataPackage);
            }, new KeyboardAccelerator() { Modifiers = Windows.System.VirtualKeyModifiers.Control, Key = Windows.System.VirtualKey.C });
            AppendItem(fileItem, LocalizationHelper.Localize("Paste URL"), async (sender, arg) =>
            {
                DataPackageView dataPackageView = Clipboard.GetContent();
                if (dataPackageView.Contains(StandardDataFormats.Text))
                {
                    string text = await dataPackageView.GetTextAsync();
                    mAppCore.GoToURL(text);
                }
            }, new KeyboardAccelerator() { Modifiers = Windows.System.VirtualKeyModifiers.Control, Key = Windows.System.VirtualKey.V });

            fileItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(fileItem, LocalizationHelper.Localize("Open Custom Folder"), async (sender, arg) =>
            {
                await Launcher.LaunchFolderAsync(Windows.Storage.ApplicationData.Current.LocalFolder);
            });

            fileItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(fileItem, LocalizationHelper.Localize("Exit"), (sender, arg) =>
            {
                Application.Current.Exit();
            });

            var navigationItem = CreateMenuBarItem(LocalizationHelper.Localize("Navigation"));

            AppendItem(navigationItem, LocalizationHelper.Localize("Select Sol"), (sender, arg) =>
            {
                mAppCore.CharEnter(104);
            }, new KeyboardAccelerator() { Key = Windows.System.VirtualKey.H, IsEnabled = false });
            AppendItem(navigationItem, LocalizationHelper.Localize("Tour Guide"), (sender, arg) =>
            {
                ShowTourGuide();
            });
            AppendItem(navigationItem, LocalizationHelper.Localize("Select Object"), (sender, arg) =>
            {
                ShowSelectObject();
            });
            AppendItem(navigationItem, LocalizationHelper.Localize("Go to Object"), (sender, arg) =>
            {
                ShowGotoObject();
            });

            navigationItem.Items.Add(new MenuFlyoutSeparator());

            var actions = new (string, short)[] {
                    ("Center Selection", 99),
                    ("Go to Selection", 103),
                    ("Follow Selection", 102),
                    ("Sync Orbit Selection", 121),
                    ("Track Selection", 116)
                };
            foreach (var action in actions)
            {
                AppendItem(navigationItem, LocalizationHelper.Localize(action.Item1), (sender, arg) =>
                {
                    mAppCore.CharEnter(action.Item2);
                }, new KeyboardAccelerator() { Key = (Windows.System.VirtualKey)(action.Item2 - 32), IsEnabled = false });
            }
            navigationItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(navigationItem, LocalizationHelper.Localize("Browser"), (sender, arg) =>
            {
                ShowBrowser();
            });
            AppendItem(navigationItem, LocalizationHelper.Localize("Eclipse Finder"), (sender, arg) =>
            {
                ShowEclipseFinder();
            });

            var timeItem = CreateMenuBarItem(LocalizationHelper.Localize("Time"));
            AppendItem(timeItem, LocalizationHelper.Localize("10x Faster"), (sender, arg) =>
            {
                mAppCore.CharEnter(108);
            }, new KeyboardAccelerator() { Key = Windows.System.VirtualKey.L, IsEnabled = false });
            AppendItem(timeItem, LocalizationHelper.Localize("10x Slower"), (sender, arg) =>
            {
                mAppCore.CharEnter(107);
            }, new KeyboardAccelerator() { Key = Windows.System.VirtualKey.K, IsEnabled = false });
            AppendItem(timeItem, LocalizationHelper.Localize("Freeze"), (sender, arg) =>
            {
                mAppCore.CharEnter(32);
            }, new KeyboardAccelerator() { Key = Windows.System.VirtualKey.Space, IsEnabled = false });
            AppendItem(timeItem, LocalizationHelper.Localize("Real Time"), (sender, arg) =>
            {
                mAppCore.CharEnter(33);
            });
            AppendItem(timeItem, LocalizationHelper.Localize("Reverse Time"), (sender, arg) =>
            {
                mAppCore.CharEnter(106);
            }, new KeyboardAccelerator() { Key = Windows.System.VirtualKey.J, IsEnabled = false });

            timeItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(timeItem, LocalizationHelper.Localize("Set Time…"), (sender, arg) =>
            {
                ShowTimeSetting();
            });

            var renderItem = CreateMenuBarItem(LocalizationHelper.Localize("Render"));
            AppendItem(renderItem, LocalizationHelper.Localize("Settings"), (sender, arg) =>
            {
                ShowViewOptions();
            });
            renderItem.Items.Add(new MenuFlyoutSeparator());
            AppendItem(renderItem, LocalizationHelper.Localize("More Stars Visible"), (sender, arg) =>
            {
                mAppCore.CharEnter(93);
            });
            AppendItem(renderItem, LocalizationHelper.Localize("Fewer Stars Visible"), (sender, arg) =>
            {
                mAppCore.CharEnter(91);
            });
            var starStyleItem = new MenuFlyoutSubItem
            {
                Text = LocalizationHelper.Localize("Star Style")
            };
            AppendSubItem(starStyleItem, LocalizationHelper.Localize("Fuzzy Points"), (sender, arg) =>
            {
                mAppCore.StarStyle = 0;
            });
            AppendSubItem(starStyleItem, LocalizationHelper.Localize("Points"), (sender, arg) =>
            {
                mAppCore.StarStyle = 1;
            });
            AppendSubItem(starStyleItem, LocalizationHelper.Localize("Scaled Discs"), (sender, arg) =>
            {
                mAppCore.StarStyle = 2;
            });
            renderItem.Items.Add(starStyleItem);
            renderItem.Items.Add(new MenuFlyoutSeparator());
            var resolutionItem = new MenuFlyoutSubItem
            {
                Text = LocalizationHelper.Localize("Texture Resolution")
            };
            AppendSubItem(resolutionItem, LocalizationHelper.Localize("Low"), (sender, arg) =>
            {
                mAppCore.Resolution = 0;
            });
            AppendSubItem(resolutionItem, LocalizationHelper.Localize("Medium"), (sender, arg) =>
            {
                mAppCore.Resolution = 1;
            });
            AppendSubItem(resolutionItem, LocalizationHelper.Localize("High"), (sender, arg) =>
            {
                mAppCore.Resolution = 2;
            });
            renderItem.Items.Add(resolutionItem);

            var viewItem = CreateMenuBarItem(LocalizationHelper.Localize("Views"));
            AppendItem(viewItem, LocalizationHelper.Localize("Split Horizontally"), (sender, arg) =>
            {
                mAppCore.CharEnter(18);
            }, new KeyboardAccelerator() { Modifiers = Windows.System.VirtualKeyModifiers.Control, Key = Windows.System.VirtualKey.R, IsEnabled = false });
            AppendItem(viewItem, LocalizationHelper.Localize("Split Vertically"), (sender, arg) =>
            {
                mAppCore.CharEnter(21);
            }, new KeyboardAccelerator() { Modifiers = Windows.System.VirtualKeyModifiers.Control, Key = Windows.System.VirtualKey.U, IsEnabled = false });
            AppendItem(viewItem, LocalizationHelper.Localize("Delete Active View"), (sender, arg) =>
            {
                mAppCore.CharEnter(127);
            }, new KeyboardAccelerator() { Key = Windows.System.VirtualKey.Delete, IsEnabled = false });
            AppendItem(viewItem, LocalizationHelper.Localize("Delete Other Views"), (sender, arg) =>
            {
                mAppCore.CharEnter(4);
            }, new KeyboardAccelerator() { Modifiers = Windows.System.VirtualKeyModifiers.Control, Key = Windows.System.VirtualKey.D, IsEnabled = false });

            var bookmarkItem = CreateMenuBarItem(LocalizationHelper.Localize("Bookmarks"));
            AppendItem(bookmarkItem, LocalizationHelper.Localize("Add Bookmark"), (sender, arg) =>
            {
                ShowNewBookmark();
            });
            AppendItem(bookmarkItem, LocalizationHelper.Localize("Organize Bookmarks…"), (sender, arg) =>
            {
                ShowBookmarkOrganizer();
            });

            var helpItem = CreateMenuBarItem(LocalizationHelper.Localize("Help"));
            AppendItem(helpItem, LocalizationHelper.Localize("Run Demo"), (sender, arg) =>
            {
                mAppCore.CharEnter(100);
            });
            helpItem.Items.Add(new MenuFlyoutSeparator());
            AppendItem(helpItem, LocalizationHelper.Localize("OpenGL Info"), (sender, arg) =>
            {
                ShowOpenGLInfo();
            });
            helpItem.Items.Add(new MenuFlyoutSeparator());
            AppendItem(helpItem, LocalizationHelper.Localize("Add-ons"), (sender, arg) =>
            {
                ShowAddons();
            });
            helpItem.Items.Add(new MenuFlyoutSeparator());
            AppendItem(helpItem, LocalizationHelper.Localize("User Guide"), (sender, arg) =>
            {
                _ = Launcher.LaunchUriAsync(new Uri("https://github.com/levinli303/Celestia/wiki"));
            });
            AppendItem(helpItem, LocalizationHelper.Localize("About Celestia"), (sender, arg) =>
            {
                ShowAboutDialog();
            });

            MenuBar.Items.Add(fileItem);
            MenuBar.Items.Add(navigationItem);
            MenuBar.Items.Add(timeItem);
            MenuBar.Items.Add(renderItem);
            MenuBar.Items.Add(viewItem);
            MenuBar.Items.Add(bookmarkItem);
            MenuBar.Items.Add(helpItem);
        }

        void AppendItem(MenuFlyout parent, string text, RoutedEventHandler click, KeyboardAccelerator accelerator = null)
        {
            var item = new MenuFlyoutItem
            {
                Text = text
            };
            if (accelerator != null)
                item.KeyboardAccelerators.Add(accelerator);
            if (click != null)
                item.Click += click;
            parent.Items.Add(item);
        }

        void AppendItem(MenuBarItem parent, string text, RoutedEventHandler click, KeyboardAccelerator accelerator = null)
        {
            var item = new MenuFlyoutItem
            {
                Text = text
            };
            if (accelerator != null)
                item.KeyboardAccelerators.Add(accelerator);
            if (click != null)
                item.Click += click;
            parent.Items.Add(item);
        }

        void AppendSubItem(MenuFlyoutSubItem parent, string text, RoutedEventHandler click)
        {
            var item = new MenuFlyoutItem
            {
                Text = text
            };
            if (click != null)
                item.Click += click;
            parent.Items.Add(item);
        }

        void AppendToggleSubItem(MenuFlyoutSubItem parent, string text, bool isChecked,  RoutedEventHandler click)
        {
            var item = new ToggleMenuFlyoutItem
            {
                IsChecked = isChecked,
                Text = text
            };
            item.Click += click;
            parent.Items.Add(item);
        }

        async void PickScript()
        {
            var picker = new Windows.Storage.Pickers.FileOpenPicker
            {
                ViewMode = Windows.Storage.Pickers.PickerViewMode.Thumbnail,
                SuggestedStartLocation = Windows.Storage.Pickers.PickerLocationId.Downloads
            };
            picker.FileTypeFilter.Add(".cel");
            picker.FileTypeFilter.Add(".celx");
            var file = await picker.PickSingleFileAsync();
            if (file != null)
                OpenFileIfReady(file);
        }
        void ShowTourGuide()
        {
            ShowPage(typeof(TourGuidePage), new Size(400, 0), mAppCore);
        }
        async void ShowSelectObject()
        {
            var dialog = new TextInputDialog(LocalizationHelper.Localize("Object name:"));
            var result = await dialog.ShowAsync();
            if (result == ContentDialogResult.Primary)
            {
                var text = dialog.Text;
                var selection = mAppCore.Simulation.Find(text);
                if (selection.IsEmpty)
                {
                    ShowObjectNotFound();
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
                    ShowObjectNotFound();
                }
                else
                {
                    var location = new CelestiaGotoLocation(selection, latitude, longitude, distance, unit);
                    mAppCore.Simulation.GoToLocation(location);
                }
            }
        }

        void ShowObjectNotFound()
        {
            ContentDialogHelper.ShowAlert(this, LocalizationHelper.Localize("Object not found."));
        }

        void ShowBrowser()
        {
            ShowPage(typeof(BrowserPage), new Size(500, 0), mAppCore);
        }

        void ShowEclipseFinder()
        {
            ShowPage(typeof(EclipseFinderPage), new Size(400, 0), mAppCore);
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
            ShowPage(typeof(ViewOptionsPage), new Size(500, 670), mAppCore);
        }

        void ShowPage(Type pageType, Size size, object parameter)
        {
            OverlayBackground.Visibility = Visibility.Visible;
            OverlayContainer.PointerPressed += (sender, arg) =>
            {
                arg.Handled = true;
            };
            OverlayBackground.PointerPressed += (sender, arg) =>
            {
                OverlayBackground.Visibility = Visibility.Collapsed;
                OverlayContainer.Content = null;
            };
            OverlayContainer.Width = size.Width;
            if (size.Height <= 1)
            {
                OverlayContainer.Height = OverlayBackground.Height;
                RelativePanel.SetAlignBottomWithPanel(OverlayContainer, true);
            }
            else
            {
                OverlayContainer.Height = size.Height;
                RelativePanel.SetAlignBottomWithPanel(OverlayContainer, false);
            }
            OverlayContainer.Navigate(pageType, parameter);
        }

        void ShowBookmarkOrganizer()
        {
            ShowPage(typeof(BookmarkOrganizerPage), new Size(400, 0), mAppCore);
        }

        void ShowNewBookmark()
        {
            ShowPage(typeof(NewBookmarkPage), new Size(400, 0), mAppCore);
        }
        async void ShowOpenGLInfo()
        {
            var dialog = new InfoDialog(mAppCore.RenderInfo)
            {
                Title = LocalizationHelper.Localize("OpenGL Info")
            };
            await dialog.ShowAsync();
        }

        void ShowAddons()
        {
            ShowPage(typeof(Addon.ResourceManagerPage), new Size(450, 0), mExtraAddonFolder);
        }

        async void ShowAboutDialog()
        {
            var dialog = new AboutDialog();
            await dialog.ShowAsync();
        }

        async Task<string> GetLocale(string LocalePath)
        {
            var folder = await Windows.Storage.StorageFolder.GetFolderFromPathAsync(LocalePath);
            var files = await folder.GetFoldersAsync();
            var availableLocales = new List<string>();
            var preferredLocale = System.Globalization.CultureInfo.CurrentUICulture.Name;
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

        private async System.Threading.Tasks.Task<Dictionary<string, object>> ReadSettings()
        {
            var installedFolder = Windows.ApplicationModel.Package.Current.InstalledLocation;
            var settingsObject = new Dictionary<string, object>();
            try
            {
                var serializer = new JsonSerializer();
                using (var st = await installedFolder.OpenStreamForReadAsync("defaults.json"))
                using (var sr = new StreamReader(st))
                using (var jsonTextReader = new JsonTextReader(sr))
                {
                    settingsObject = serializer.Deserialize<Dictionary<string, object>>(jsonTextReader);
                }
            }
            catch { }
            var newSettings = new Dictionary<string, object>();
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            foreach (var kvp in settingsObject)
            {
                var propInfo = typeof(CelestiaAppCore).GetProperty(kvp.Key);
                if (propInfo == null) continue;

                var def = kvp.Value;
                var saved = localSettings.Values[kvp.Key];
                if (saved != null)
                {
                    newSettings[kvp.Key] = saved;
                }
                else if (propInfo.PropertyType == typeof(Boolean))
                {
                    newSettings[kvp.Key] = (long)def != 0;
                }
                else if (propInfo.PropertyType == typeof(Double) || propInfo.PropertyType == typeof(Single))
                {
                    newSettings[kvp.Key] = (float)((double)def);
                }
                else if (propInfo.PropertyType == typeof(Int64) || propInfo.PropertyType == typeof(Int32) || propInfo.PropertyType == typeof(Int16))
                {
                    newSettings[kvp.Key] = (int)((long)def);
                }
            }
            return newSettings;
        }

        private Dictionary<string, object> GetCurrentSettings()
        {
            var newSettings = new Dictionary<string, object>();
            foreach (var kvp in mSettings)
            {
                var propInfo = typeof(CelestiaAppCore).GetProperty(kvp.Key);
                if (propInfo == null) continue;

                newSettings[kvp.Key] = mAppCore.GetType().GetProperty(kvp.Key).GetValue(mAppCore);
            }
            return newSettings;
        }

        private void ApplySettings(Dictionary<string, object> settings)
        {
            foreach (var kvp in settings)
            {
                mAppCore.GetType().GetProperty(kvp.Key).SetValue(mAppCore, kvp.Value);
            }
        }

        private void SaveSettings(Dictionary<string, object> settings)
        {
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            foreach (var kvp in settings)
            {
                localSettings.Values[kvp.Key] = kvp.Value;
            }
        }

        private MenuFlyoutItemBase CreateMenuItem(CelestiaBrowserItem item)
        {
            var menu = new MenuFlyoutSubItem
            {
                Text = item.Name
            };

            menu.Loaded += (sender, args) =>
            {
                var senderMenu = (MenuFlyoutSubItem)sender;
                if (senderMenu.Items.Count > 0) return;

                var children = new List<MenuFlyoutItemBase>();
                var obj = item.Object;
                if (obj != null)
                {
                    var selectItem = new MenuFlyoutItem
                    {
                        Text = LocalizationHelper.Localize("Select")
                    };
                    selectItem.Click += (newSender, newArgs) =>
                    {
                        mAppCore.Simulation.Selection = new CelestiaSelection(obj);
                    };
                    children.Add(selectItem);
                }
                if (item.Children != null)
                {
                    if (children.Count > 0)
                        children.Add(new MenuFlyoutSeparator());

                    foreach (var child in item.Children)
                    {
                        children.Add(CreateMenuItem(child));
                    }
                }
                foreach (var child in children)
                {
                    senderMenu.Items.Add(child);
                }
            };
            return menu;
        }
        private CelestiaBrowserItem[] GetChildren(CelestiaBrowserItem item)
        {
            var obj = item.Object;
            if (obj == null)
                return new CelestiaBrowserItem[] { };
            if (obj is CelestiaStar star)
                return mAppCore.Simulation.Universe.ChildrenForStar(star, GetChildren);
            if (obj is CelestiaBody body)
                return mAppCore.Simulation.Universe.ChildrenForBody(body, GetChildren);
            return new CelestiaBrowserItem[] { };
        }

        private void CaptureImage()
        {
            var tempFolder = Windows.Storage.ApplicationData.Current.TemporaryFolder;
            var path = tempFolder.Path + "\\" + GuidHelper.CreateNewGuid().ToString() + ".png";
            mRenderer.EnqueueTask(() =>
            {
                mAppCore.Draw();
                if (mAppCore.SaveScreenshot(path))
                {
                    _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                      {
                          SaveScreenshot(path);
                      });
                }
                else
                {
                    _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        ShowScreenshotFailure();
                    });
                }
            });
        }

        private void ShowScreenshotFailure()
        {
            ContentDialogHelper.ShowAlert(this, LocalizationHelper.Localize("Failed in capturing screenshot."));
        }

        private async void SaveScreenshot(string path)
        {
            try
            {
                var originalFile = await Windows.Storage.StorageFile.GetFileFromPathAsync(path);
                if (originalFile == null) return;

                var savePicker = new Windows.Storage.Pickers.FileSavePicker
                {
                    SuggestedStartLocation =
                    Windows.Storage.Pickers.PickerLocationId.DocumentsLibrary
                };
                // Dropdown of file types the user can save the file as
                savePicker.FileTypeChoices.Add(LocalizationHelper.Localize("Image"), new List<string>() { ".png" });
                // Default file name if the user does not type one in or select a file to replace
                savePicker.SuggestedFileName = LocalizationHelper.Localize("Celestia Screenshot");
                Windows.Storage.StorageFile file = await savePicker.PickSaveFileAsync();
                if (file == null) return;

                await originalFile.CopyAndReplaceAsync(file);
            }
            catch
            {
                ShowScreenshotFailure();
            }
        }
    }
}
