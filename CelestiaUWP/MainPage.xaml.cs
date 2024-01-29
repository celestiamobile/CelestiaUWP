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

using CelestiaAppComponent;
using CelestiaComponent;
using CelestiaUWP.Settings;
using CelestiaUWP.Web;
using Microsoft.AppCenter;
using Microsoft.AppCenter.Crashes;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using Windows.ApplicationModel.Core;
using Windows.ApplicationModel.DataTransfer;
using Windows.ApplicationModel.Email;
using Windows.ApplicationModel.Resources;
using Windows.Data.Json;
using Windows.Foundation;
using Windows.Globalization;
using Windows.Storage;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

using MenuBarItem = Microsoft.UI.Xaml.Controls.MenuBarItem;

namespace CelestiaUWP
{
    public sealed partial class MainPage : Page
    {
        private readonly CelestiaAppCore mAppCore;
        private CelestiaRenderer mRenderer;

        private Point? lastMousePosition = null;
        private Point? mousePressedGlobalPosition = null;
        private CelestiaMouseButton? currentPressedButton = null;

        private StorageFolder mExtraAddonFolder = null;
        private string mExtraAddonFolderPath = "";
        private StorageFolder mExtraScriptFolder = null;
        private string mExtraScriptFolderPath = "";

        private StorageFile ScriptFileToOpen;
        private Uri URLToOpen;
        private bool ReadyForInput = false;
        private bool isMouseHidden = false;

        private AppSettings _appSetting;
        private AppSettings AppSettings
        {
            get
            {
                if (_appSetting == null)
                    _appSetting = new AppSettings(ApplicationData.Current.LocalSettings);
                return _appSetting;
            }
        }

        private ResourceManager resourceManager = null;

        private bool isXbox = false;
        // Used in renderer thread
        private bool isGLViewFocused = false;
        private bool hasOverlayOpen = false;

        private string defaultParentPath
        {
            get { return Windows.ApplicationModel.Package.Current.InstalledLocation.Path; }
        }

        private string defaultResourcePath
        {
            get { return PathHelper.Combine(defaultParentPath, "CelestiaResources"); }
        }

        private string defaultConfigFilePath
        {
            get { return PathHelper.Combine(defaultResourcePath, "celestia.cfg"); }
        }

        private string[] AvailableLanguages;

        private float scale = 1.0f;

        private GamepadManager gamepadManager = null;

        private static string FeedbackEmailAddress = "celestia.mobile@outlook.com";
        private static string FeedbackGitHubLink = "https://celestia.mobi/feedback";

        public MainPage()
        {
            CelestiaAppCore.SetUpLocale();

            mAppCore = new CelestiaAppCore();

            InitializeComponent();

            Loaded += MainPage_Loaded;
            SizeChanged += MainPage_SizeChanged;

            var coreTitleBar = CoreApplication.GetCurrentView().TitleBar;
            coreTitleBar.ExtendViewIntoTitleBar = false;
            isXbox = Windows.System.Profile.AnalyticsInfo.VersionInfo.DeviceFamily == "Windows.Xbox";
        }

        private void MainPage_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (!ReadyForInput || isXbox) return;

            var isFullScreen = ApplicationView.GetForCurrentView().IsFullScreenMode;
            MenuBar.Visibility = isFullScreen ? Visibility.Collapsed : Visibility.Visible;
        }

        private async void MainPage_Loaded(object sender, RoutedEventArgs e)
        {
            scale = AppSettings.UseFullDPI ? ((int)Windows.Graphics.Display.DisplayInformation.GetForCurrentView().ResolutionScale) / 100.0f : 1.0f;

            StorageFolder customDataFolder = null;
            StorageFile customConfigFile = null;
            try
            {
                customDataFolder = await ApplicationData.Current.LocalFolder.GetFolderAsync("Override");
                customConfigFile = await customDataFolder.GetFileAsync("celestia.cfg");
            }
            catch { }

            var resourcePath = customDataFolder != null ? customDataFolder.Path : defaultResourcePath;
            var configPath = customConfigFile != null ? customConfigFile.Path : defaultConfigFilePath;

            var localePath = PathHelper.Combine(defaultResourcePath, "locale");

            // Migrate override language to system
            var overrideLocaleLegacy = AppSettings.LanguageOverride;
            if (overrideLocaleLegacy != null && overrideLocaleLegacy != "")
            {
                ApplicationLanguages.PrimaryLanguageOverride = LocalizationHelper.ToWindowsTag(overrideLocaleLegacy);
                AppSettings.LanguageOverride = "";
                AppSettings.Save(ApplicationData.Current.LocalSettings);
                Windows.ApplicationModel.Resources.Core.ResourceContext.GetForCurrentView().Reset();
                Windows.ApplicationModel.Resources.Core.ResourceContext.GetForViewIndependentUse().Reset();
            }

            var resourceLoader = ResourceLoader.GetForViewIndependentUse();
            var flowDirection = resourceLoader.GetString("ApplicationFlowDirection");
            FlowDirection = flowDirection == "RightToLeft" ? FlowDirection.RightToLeft : FlowDirection.LeftToRight;

            var locale = await GetLocale(localePath);
            await Task.Run(() => CreateExtraFolders());

            mRenderer = new CelestiaRenderer(AppSettings.EnableMSAA, () => {
                CelestiaAppCore.InitGL();

                List<string> extraPaths = new List<string>();
                if (mExtraAddonFolderPath.Length > 0)
                    extraPaths.Add(mExtraAddonFolderPath);

                void progressCallback(string progress)
                {
                    _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        LoadingText.Text = string.Format(LocalizationHelper.Localize("Loading: %s").Replace("%s", "{0}"), progress);
                    });
                }

                Directory.SetCurrentDirectory(resourcePath);
                CelestiaAppCore.SetLocaleDirectory(PathHelper.Combine(resourcePath, "locale"), locale);
                if (!mAppCore.StartSimulation(configPath, extraPaths.ToArray(), progressCallback) && (resourcePath != defaultResourcePath || configPath != defaultConfigFilePath))
                {
                    if (resourcePath != defaultResourcePath || configPath != defaultConfigFilePath)
                    {
                        // Try to restore originial settings
                        _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
                        {
                            await ContentDialogHelper.ShowAlert(this, LocalizationHelper.Localize("Error loading data, fallback to original configuration."));
                        });
                        Directory.SetCurrentDirectory(defaultResourcePath);
                        CelestiaAppCore.SetLocaleDirectory(PathHelper.Combine(defaultResourcePath, "locale"), locale);
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

                if (!mAppCore.StartRenderer())
                {
                    _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        ShowLoadingFailure();
                    });
                    return false;
                }

                LocalizationHelper.Locale = CelestiaAppCore.Language;
                mAppCore.LayoutDirection = flowDirection == "RightToLeft" ? CelestiaLayoutDirection.RTL : CelestiaLayoutDirection.LTR;
                UpdateScale();

                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    LoadingText.Visibility = Visibility.Collapsed;
                    if (mExtraAddonFolder != null)
                        resourceManager = new CelestiaAppComponent.ResourceManager(mExtraAddonFolder, mExtraScriptFolder);
                    SetUpGLViewInteractions();
                    PopulateMenuBar(resourcePath);
                });

                ApplySettings(ReadDefaultSettings().Result);

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
            mRenderer.Start();
        }

        private void UpdateScale()
        {
            mAppCore.SetDPI((int)(96.0f * scale));
            mAppCore.PickTolerance = (float)AppSettings.PickSensitivity * scale;
            var fontMap = new Dictionary<string, (string, int, string, int)>()
                {
                    { "ja", ("NotoSansCJK-Regular.ttc", 0, "NotoSansCJK-Bold.ttc", 0) },
                    { "ko", ("NotoSansCJK-Regular.ttc", 1, "NotoSansCJK-Bold.ttc", 1) },
                    { "ka", ("NotoSansGeorgian-Regular.ttf", 0, "NotoSansGeorgian-Bold.ttf", 0) },
                    { "zh_CN", ("NotoSansCJK-Regular.ttc", 2, "NotoSansCJK-Bold.ttc", 2) },
                    { "zh_TW", ("NotoSansCJK-Regular.ttc", 3, "NotoSansCJK-Bold.ttc", 3) },
                    { "ar", ("NotoSansArabic-Regular.ttf", 0, "NotoSansArabic-Bold.ttf", 0) },
                };
            var defaultFont = ("NotoSans-Regular.ttf", 0, "NotoSans-Bold.ttf", 0);
            var font = fontMap.GetValueOrDefault(LocalizationHelper.Locale, defaultFont);
            var textScaleFactor = new UISettings().TextScaleFactor;
            var pathPrefix = PathHelper.Combine(defaultResourcePath, "fonts");
            mAppCore.SetHudFont(PathHelper.Combine(pathPrefix, font.Item1), font.Item2, (int)(9 * textScaleFactor));
            mAppCore.SetHudTitleFont(PathHelper.Combine(pathPrefix, font.Item3), font.Item4, (int)(15 * textScaleFactor));
            mAppCore.SetRenderFont(PathHelper.Combine(pathPrefix, font.Item1), font.Item2, (int)(9 * textScaleFactor), CelestiaFontStyle.Normal);
            mAppCore.SetRenderFont(PathHelper.Combine(pathPrefix, font.Item3), font.Item4, (int)(15 * textScaleFactor), CelestiaFontStyle.Large);
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
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.RunScript(copiedFile.Path);
                        });
                    }
                }
                catch { }
                return;
            }
            if (url != null)
            {
                URLToOpen = null;
                if (resourceManager != null && url.Scheme == "celaddon" && url.Host == "item" && url.Query != null)
                {
                    var query = System.Web.HttpUtility.ParseQueryString(url.Query);
                    var addon = query["item"];
                    if (addon == null) { return; }

                    Windows.Web.Http.HttpClient httpClient = new Windows.Web.Http.HttpClient();
                    var queryItems = System.Web.HttpUtility.ParseQueryString("");
                    queryItems.Add("lang", LocalizationHelper.Locale);
                    queryItems.Add("item", addon);
                    var builder = new UriBuilder(Addon.Constants.APIPrefix + "/resource/item");
                    builder.Query = queryItems.ToString();
                    try
                    {
                        var httpResponse = await httpClient.GetAsync(builder.Uri);
                        httpResponse.EnsureSuccessStatusCode();
                        var httpResponseBody = await httpResponse.Content.ReadAsStringAsync();
                        var requestResult = RequestResult.TryParse(httpResponseBody);
                        if (requestResult.Status == 0)
                        {
                            var item = ResourceItem.TryParse(requestResult.Info.Detail);
                            ShowPage(typeof(Addon.ResourceItemPage), new Size(450, 0), new Addon.AddonPageParameter(mAppCore, mRenderer, item, resourceManager));
                        }
                    }
                    catch { }
                    return;
                }
                else if (url.Scheme == "celguide" && url.Host == "guide" && url.Query != null)
                {
                    var query = System.Web.HttpUtility.ParseQueryString(url.Query);
                    var guide = query["guide"];
                    if (guide == null) return;
                    ShowPage(typeof(SafeWebPage), new Size(450, 0), GenerateWebArgsForGuide(guide));
                    return;
                }
                else if (url.Scheme == "cel")
                {
                    if (await ContentDialogHelper.ShowOption(this, LocalizationHelper.Localize("Open URL?")))
                    {
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.GoToURL(url.AbsoluteUri);
                        });
                    }
                    return;
                }
            }

            if (ShowHelpIfNeeded())
            {
                return;
            }

            {
                Windows.Web.Http.HttpClient httpClient = new Windows.Web.Http.HttpClient();
                var queryItems = System.Web.HttpUtility.ParseQueryString("");
                queryItems.Add("lang", LocalizationHelper.Locale);
                queryItems.Add("type", "news");
                var builder = new UriBuilder(Addon.Constants.APIPrefix + "/resource/latest");
                builder.Query = queryItems.ToString();

                try
                {
                    var httpResponse = await httpClient.GetAsync(builder.Uri);
                    httpResponse.EnsureSuccessStatusCode();
                    var httpResponseBody = await httpResponse.Content.ReadAsStringAsync();
                    var requestResult = RequestResult.TryParse(httpResponseBody);
                    if (requestResult.Status == 0)
                    {
                        var item = GuideItem.TryParse(requestResult.Info.Detail);
                        var appSettings = AppSettings;
                        if (item.ID != appSettings.LastNewsID)
                        {
                            var args = GenerateWebArgsForGuide(item.ID);
                            args.ACKReceiver = (id) =>
                            {
                                if (id == item.ID)
                                {
                                    appSettings.LastNewsID = id;
                                    appSettings.Save(ApplicationData.Current.LocalSettings);
                                }
                            };
                            ShowPage(typeof(SafeWebPage), new Size(450, 0), args);
                            return;
                        }
                    }
                }
                catch { }
            }
        }

        public CommonWebArgs GenerateWebArgsForGuide(string id)
        {
            var queryItems = System.Web.HttpUtility.ParseQueryString("");
            queryItems.Add("lang", LocalizationHelper.Locale);
            queryItems.Add("guide", id);
            queryItems.Add("platform", isXbox ? "xbox" : "uwp");
            queryItems.Add("transparentBackground", "1");
            var builder = new UriBuilder("https://celestia.mobi/resources/guide");
            builder.Query = queryItems.ToString();
            var args = new CommonWebArgs();
            args.Renderer = mRenderer;
            args.AppCore = mAppCore;
            args.Uri = builder.Uri;
            args.MatchingQueryKeys = new string[] { "guide" };
            return args;
        }

        public CommonWebArgs GenerateWebArgsForPath(string path)
        {
            // Same additional parameters here
            var queryItems = System.Web.HttpUtility.ParseQueryString("");
            queryItems.Add("lang", LocalizationHelper.Locale);
            queryItems.Add("platform", isXbox ? "xbox" : "uwp");
            queryItems.Add("transparentBackground", "1");
            var builder = new UriBuilder($"https://celestia.mobi{path}");
            builder.Query = queryItems.ToString();
            var args = new CommonWebArgs();
            args.Renderer = mRenderer;
            args.AppCore = mAppCore;
            args.Uri = builder.Uri;
            args.MatchingQueryKeys = new string[] {};
            return args;
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
                mExtraAddonFolder = await mainFolder.CreateFolderAsync("extras", Windows.Storage.CreationCollisionOption.OpenIfExists);
                mExtraAddonFolderPath = mExtraAddonFolder.Path;
                mExtraScriptFolder = await mainFolder.CreateFolderAsync("scripts", Windows.Storage.CreationCollisionOption.OpenIfExists);
                mExtraScriptFolderPath = mExtraScriptFolder.Path;
            } catch { }
        }

        void SetUpGLViewInteractions()
        {
            // mAppCore.ChangeCursor += AppCore_ChangeCursor; // Not useful for now since we only capture pointer on press
            mAppCore.FatalError += AppCore_FatalError;
            mAppCore.ShowContextMenu += (_, contextMenuArgs) =>
            {
                var x = contextMenuArgs.X;
                var y = contextMenuArgs.Y;
                var selection = contextMenuArgs.Selection;
                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                  {
                      var menu = new MenuFlyout();
                      AppendItem(menu, mAppCore.Simulation.Universe.NameForSelection(selection), null);
                      menu.Items.Add(new MenuFlyoutSeparator());
                      AppendItem(menu, LocalizationHelper.Localize("Get Info"), (sender, arg) =>
                      {
                          ShowInfo(selection);
                      });

                      menu.Items.Add(new MenuFlyoutSeparator());

                      var actions = new (string, short)[]
                      {
                        (LocalizationHelper.Localize("Go"), 103),
                        (LocalizationHelper.Localize("Follow"), 102),
                        (LocalizationHelper.Localize("Sync Orbit"), 121),
                      };

                      foreach (var action in actions)
                      {
                          AppendItem(menu, action.Item1, (sender, arg) =>
                          {
                              mRenderer.EnqueueTask(() =>
                              {
                                  mAppCore.Simulation.Selection = selection;
                                  mAppCore.CharEnter(action.Item2);
                              });
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
                                  mRenderer.EnqueueTask(() =>
                                  {
                                      mAppCore.Simulation.ActiveObserver.DisplayedSurfaceName = "";
                                  });
                              });

                              foreach (var name in surfaces)
                              {
                                  AppendSubItem(altSur, name, (sender, arg) =>
                                  {
                                      mRenderer.EnqueueTask(() =>
                                      {
                                          mAppCore.Simulation.ActiveObserver.DisplayedSurfaceName = name;
                                      });
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
                              (LocalizationHelper.Localize("Show Body Axes"), "ShowBodyAxes"),
                              (LocalizationHelper.Localize("Show Frame Axes"), "ShowFrameAxes"),
                              (LocalizationHelper.Localize("Show Sun Direction"), "ShowSunDirection"),
                              (LocalizationHelper.Localize("Show Velocity Vector"), "ShowVelocityVector"),
                              (LocalizationHelper.Localize("Show Planetographic Grid"), "ShowPlanetographicGrid"),
                              (LocalizationHelper.Localize("Show Terminator"), "ShowTerminator")
                          };

                          foreach (var action in refMarkMenuActions)
                          {
                              var isEnabled = (bool)mAppCore.GetType().GetProperty(action.Item2).GetValue(mAppCore);
                              AppendToggleSubItem(refMarkMenu, action.Item1, isEnabled, (sender, args) =>
                              {
                                  mRenderer.EnqueueTask(() =>
                                  {
                                      mAppCore.GetType().GetProperty(action.Item2).SetValue(mAppCore, !isEnabled);
                                  });
                              });
                          }
                          menu.Items.Add(refMarkMenu);
                      }

                      var browserMenuItems = new List<MenuFlyoutItemBase>();
                      var browserItem = new CelestiaBrowserItem(mAppCore.Simulation.Universe.NameForSelection(selection), selection.Object, (CelestiaBrowserItem item) => { return CelestiaExtension.GetChildren(item, mAppCore); }, false);
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

                      menu.Items.Add(new MenuFlyoutSeparator());

                      if (mAppCore.Simulation.Universe.IsSelectionMarked(selection))
                      {
                          AppendItem(menu, LocalizationHelper.Localize("Unmark"), (sender, arg) =>
                          {
                              mRenderer.EnqueueTask(() =>
                              {
                                  mAppCore.Simulation.Universe.UnmarkSelection(selection);
                              });
                          });
                      }
                      else
                      {
                          string[] markers = new string[]
                          {
                              LocalizationHelper.Localize("Diamond"),
                              LocalizationHelper.Localize("Triangle"),
                              LocalizationHelper.Localize("Filled Square"),
                              LocalizationHelper.Localize("Plus"),
                              LocalizationHelper.Localize("X"),
                              LocalizationHelper.Localize("Left Arrow"),
                              LocalizationHelper.Localize("Right Arrow"),
                              LocalizationHelper.Localize("Up Arrow"),
                              LocalizationHelper.Localize("Down Arrow"),
                              LocalizationHelper.Localize("Circle"),
                              LocalizationHelper.Localize("Disk"),
                              LocalizationHelper.Localize("Crosshair")
                          };
                          var action = new MenuFlyoutSubItem
                          {
                              Text = LocalizationHelper.Localize("Mark")
                          };
                          for (int i = 0; i < markers.Length; i += 1)
                          {
                              int copy = i;
                              AppendSubItem(action, markers[i], (sender, arg) =>
                              {
                                  mRenderer.EnqueueTask(() =>
                                  {
                                      mAppCore.Simulation.Universe.MarkSelection(selection, (CelestiaMarkerRepresentation)copy);
                                      mAppCore.ShowMarkers = true;
                                  });
                              });
                          }
                          menu.Items.Add(action);
                      }
                      menu.ShowAt(GLView, new Point(x / scale, y / scale));
                  });
            };
            GLView.PointerPressed += (sender, args) =>
            {
                if (args.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Mouse)
                {
                    var properties = args.GetCurrentPoint((UIElement)sender).Properties;
                    CelestiaMouseButton? newButtonPressed = null;
                    if (properties.IsLeftButtonPressed && currentPressedButton != CelestiaMouseButton.Left)
                    {
                        newButtonPressed = CelestiaMouseButton.Left;
                    }
                    else if (properties.IsRightButtonPressed && currentPressedButton != CelestiaMouseButton.Right)
                    {
                        newButtonPressed = CelestiaMouseButton.Right;
                    }
                    if (properties.IsMiddleButtonPressed && currentPressedButton != CelestiaMouseButton.Middle)
                    {
                        newButtonPressed = CelestiaMouseButton.Middle; 
                    }
                    if (newButtonPressed != null)
                    {
                        var oldButton = currentPressedButton;
                        currentPressedButton = newButtonPressed;
                        var position = args.GetCurrentPoint((UIElement)sender).Position;
                        var scaledPosition = new Point(position.X * scale, position.Y * scale);

                        mRenderer.EnqueueTask(() =>
                        {
                            if (oldButton != null)
                                mAppCore.MouseButtonUp((float)scaledPosition.X, (float)scaledPosition.Y, (CelestiaMouseButton)oldButton);
                            mAppCore.MouseButtonDown((float)scaledPosition.X, (float)scaledPosition.Y, (CelestiaMouseButton)newButtonPressed);
                        });

                        lastMousePosition = position;
                        mousePressedGlobalPosition = CoreWindow.GetForCurrentThread().PointerPosition;
                        if (!isMouseHidden)
                        {
                            isMouseHidden = true;
                            GLView.CapturePointer(args.Pointer);
                            CoreWindow.GetForCurrentThread().PointerCursor = null;
                        }
                    }
                }
            };
            GLView.PointerMoved += (sender, args) =>
            {
                if (args.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Mouse)
                {
                    if (lastMousePosition == null || currentPressedButton == null || mousePressedGlobalPosition == null)
                        return;

                    var newPosition = args.GetCurrentPoint((UIElement)sender).Position;
                    var scaledPosition = new Point(newPosition.X * scale, newPosition.Y * scale);
                    var oldScaledPosition = new Point(((Point)lastMousePosition).X * scale, ((Point)lastMousePosition).Y * scale);
                    var x = scaledPosition.X - oldScaledPosition.X;
                    var y = scaledPosition.Y - oldScaledPosition.Y;
                    var button = (CelestiaMouseButton)currentPressedButton;
                    mRenderer.EnqueueTask(() =>
                    {
                        mAppCore.MouseMove((float)x, (float)y, button);
                    });
                    CoreWindow.GetForCurrentThread().PointerPosition = (Point)mousePressedGlobalPosition;
                }
            };
            GLView.PointerReleased += (sender, args) =>
            {
                if (args.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Mouse)
                {
                    var properties = args.GetCurrentPoint((UIElement)sender).Properties;
                    bool buttonReleased = false;
                    if (currentPressedButton == CelestiaMouseButton.Left)
                    {
                        buttonReleased = !properties.IsLeftButtonPressed;
                    }
                    else if (currentPressedButton == CelestiaMouseButton.Right)
                    {
                        buttonReleased = !properties.IsRightButtonPressed;
                    }
                    else if (currentPressedButton == CelestiaMouseButton.Middle)
                    {
                        buttonReleased = !properties.IsMiddleButtonPressed;
                    }

                    if (buttonReleased)
                    {
                        var position = args.GetCurrentPoint((UIElement)sender).Position;
                        var scaledPosition = new Point(position.X * scale, position.Y * scale);
                        var button = (CelestiaMouseButton)currentPressedButton;
                        mRenderer.EnqueueTask(() =>
                        {
                            mAppCore.MouseButtonUp((float)scaledPosition.X, (float)scaledPosition.Y, button);
                        });
                        currentPressedButton = null;
                        lastMousePosition = null;
                        mousePressedGlobalPosition = null;

                        if (isMouseHidden)
                        {
                            isMouseHidden = false;
                            GLView.ReleasePointerCapture(args.Pointer);
                            CoreWindow.GetForCurrentThread().PointerCursor = new CoreCursor(CoreCursorType.Arrow, 0);
                        }
                    }
                }
            };
            GLView.PointerWheelChanged += (sender, arg) =>
            {
                var delta = arg.GetCurrentPoint((UIElement)sender).Properties.MouseWheelDelta;
                mRenderer.EnqueueTask(() =>
                {
                    mAppCore.MouseWheel(delta > 0 ? -1 : 1, 0);
                });
            };
            FocusHelperControl.Focus(FocusState.Programmatic);
            isGLViewFocused = true;
            FocusManager.GotFocus += FocusManager_GotFocus;
            FocusHelperControl.CharacterReceived += (sender, arg) =>
            {
                if (OverlayContainer.Content != null) return;
                short key = (short)arg.Character;

                mRenderer.EnqueueTask(() =>
                {
                    mAppCore.CharEnter(key);
                });
            };
            FocusHelperControl.KeyDown += (sender, arg) =>
            {
                if (OverlayContainer.Content != null) return;
                // Gamepad buttons, ignore
                if (arg.OriginalKey >= VirtualKey.GamepadA && arg.OriginalKey <= VirtualKey.GamepadRightThumbstickLeft) return;

                var modifiers = 0;
                if (CoreWindow.GetForCurrentThread().GetKeyState(VirtualKey.Control) == CoreVirtualKeyStates.Down)
                    modifiers |= 16;
                if (CoreWindow.GetForCurrentThread().GetKeyState(VirtualKey.Shift) == CoreVirtualKeyStates.Down)
                    modifiers |= 8;
                int key = (int)arg.Key;

                mRenderer.EnqueueTask(() =>
                {
                    mAppCore.KeyDown(key, modifiers);
                });
            };
            FocusHelperControl.KeyUp += FocusHelperControl_KeyUp;
            MenuBar.KeyUp += (_, args) =>
            {
                if (isXbox && args.Key == VirtualKey.GamepadMenu && OverlayContainer.Content == null)
                {
                    MenuBar.Visibility = Visibility.Collapsed;
                    FocusHelperControl.Focus(FocusState.Programmatic);
                }
            };
            gamepadManager = new GamepadManager(AppSettings);
            mRenderer.SetPreRenderTask(() =>
            {
                if (!isGLViewFocused || hasOverlayOpen) return;
                gamepadManager.PollGamepad(mAppCore);
            });
            SystemNavigationManager.GetForCurrentView().BackRequested += (sender, args) =>
            {
                if (OverlayContainer.Content != null)
                {
                    args.Handled = true;
                    ClosePanel();
                    return;
                }
                if (MenuBar.Visibility == Visibility.Visible)
                {
                    args.Handled = true;
                    MenuBar.Visibility = Visibility.Collapsed;
                    FocusHelperControl.Focus(FocusState.Programmatic);
                    return;
                }
                args.Handled = true;
            };
        }

        private void AppCore_ChangeCursor(object sender, ChangeCursorArgs e)
        {
            _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                var window = CoreWindow.GetForCurrentThread();
                switch (e.Cursor)
                {
                    case Cursor.Arrow:
                        window.PointerCursor = new CoreCursor(CoreCursorType.Arrow, 0);
                        break;
                    case Cursor.UpArrow:
                        window.PointerCursor = new CoreCursor(CoreCursorType.UpArrow, 0);
                        break;
                    case Cursor.Cross:
                        window.PointerCursor = new CoreCursor(CoreCursorType.Cross, 0);
                        break;
                    case Cursor.InvertedCross:
                        window.PointerCursor = new CoreCursor(CoreCursorType.Cross, 0);
                        break;
                    case Cursor.Wait:
                        window.PointerCursor = new CoreCursor(CoreCursorType.Wait, 0);
                        break;
                    case Cursor.Busy:
                        window.PointerCursor = new CoreCursor(CoreCursorType.Wait, 0);
                        break;
                    case Cursor.Ibeam:
                        window.PointerCursor = new CoreCursor(CoreCursorType.IBeam, 0);
                        break;
                    case Cursor.SizeVer:
                        window.PointerCursor = new CoreCursor(CoreCursorType.SizeNorthSouth, 0);
                        break;
                    case Cursor.SizeHor:
                        window.PointerCursor = new CoreCursor(CoreCursorType.SizeWestEast, 0);
                        break;
                    case Cursor.SizeBDiag:
                        window.PointerCursor = new CoreCursor(CoreCursorType.SizeNorthwestSoutheast, 0);
                        break;
                    case Cursor.SizeFDiag:
                        window.PointerCursor = new CoreCursor(CoreCursorType.SizeNortheastSouthwest, 0);
                        break;
                    case Cursor.SizeAll:
                        window.PointerCursor = new CoreCursor(CoreCursorType.SizeAll, 0);
                        break;
                    case Cursor.SplitV:
                        window.PointerCursor = new CoreCursor(CoreCursorType.SizeWestEast, 0);
                        break;
                    case Cursor.SplitH:
                        window.PointerCursor = new CoreCursor(CoreCursorType.SizeNorthSouth, 0);
                        break;
                    case Cursor.PointingHand:
                        window.PointerCursor = new CoreCursor(CoreCursorType.Hand, 0);
                        break;
                    case Cursor.Forbidden:
                        window.PointerCursor = new CoreCursor(CoreCursorType.UniversalNo, 0);
                        break;
                    case Cursor.WhatsThis:
                        window.PointerCursor = new CoreCursor(CoreCursorType.Help, 0);
                        break;
                }
            });
        }

        private void AppCore_FatalError(object sender, FatalErrorArgs e)
        {
            _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, async () =>
            {
                await ContentDialogHelper.ShowAlert(this, e.Message);
            });
        }

        private void FocusHelperControl_KeyUp(object sender, KeyRoutedEventArgs args)
        {
            if (OverlayContainer.Content != null) return;
            if (isXbox)
            {
                if (args.Key == VirtualKey.GamepadMenu)
                {
                    // Show menu
                    MenuBar.Visibility = Visibility.Visible;
                    if (MenuBar.Items[0].IsLoaded)
                        MenuBar.Items[0].Focus(FocusState.Programmatic);
                    else
                        MenuBar.Items[0].Loaded += (s, a) =>
                        {
                            ((MenuBarItem)s).Focus(FocusState.Programmatic);
                        };
                    return;
                }
            }

            // Gamepad buttons, ignore
            if (args.OriginalKey >= VirtualKey.GamepadA && args.OriginalKey <= VirtualKey.GamepadRightThumbstickLeft) return;

            int key = (int)args.Key;

            mRenderer.EnqueueTask(() =>
            {
                mAppCore.KeyUp(key, 0);
            });
        }

        private void FocusManager_GotFocus(object sender, FocusManagerGotFocusEventArgs e)
        {
            var focusedItem = e.NewFocusedElement;
            var glViewHasFocus = focusedItem == FocusHelperControl;
            if (!glViewHasFocus)
            {
                var element = focusedItem as FrameworkElement;
                if (element != null)
                {
                    // Do not give up focus to GLView and its parents
                    FrameworkElement parent = GLView;
                    while (parent != null && parent != element)
                    {
                        var potentialParent = parent.Parent;
                        if (potentialParent != null)
                            parent = potentialParent as FrameworkElement;
                        else
                            parent = null;
                    }

                    if (parent == element)
                    {
                        FocusHelperControl.Focus(FocusState.Programmatic);
                        glViewHasFocus = true;
                    }
                }
            }
            mRenderer.EnqueueTask(() =>
                {
                    isGLViewFocused = glViewHasFocus;
                });
        }

        void PopulateMenuBar(string resourcePath)
        {
            MenuBarItem CreateMenuBarItem(string name)
            {
                var item = new MenuBarItem
                {
                    Title = name,
                };
                return item;
            }

            var isFullScreen = ApplicationView.GetForCurrentView().IsFullScreenMode;
            MenuBar.Visibility = (isFullScreen || isXbox) ? Visibility.Collapsed : Visibility.Visible;

            var fileItem = CreateMenuBarItem(LocalizationHelper.Localize("File"));

            AppendItem(fileItem, LocalizationHelper.Localize("Run Script…"), (sender, arg) =>
            {
                PickScript();
            });

            var scriptsItem = new MenuFlyoutSubItem
            {
                Text = LocalizationHelper.Localize("Scripts")
            };
            var scripts = CelestiaAppCore.ReadScripts(PathHelper.Combine(resourcePath, "scripts"), true);
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
                var extraScripts = CelestiaAppCore.ReadScripts(mExtraScriptFolderPath, true);
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
            }, new KeyboardAccelerator() { Key = VirtualKey.F10 });

            fileItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(fileItem, LocalizationHelper.Localize("Copy URL"), (sender, arg) =>
            {
                DataPackage dataPackage = new DataPackage
                {
                    RequestedOperation = DataPackageOperation.Copy
                };
                dataPackage.SetText(mAppCore.CurrentURL);
                Clipboard.SetContent(dataPackage);
            }, new KeyboardAccelerator() { Modifiers = VirtualKeyModifiers.Control, Key = VirtualKey.C });
            AppendItem(fileItem, LocalizationHelper.Localize("Paste URL"), async (sender, arg) =>
            {
                DataPackageView dataPackageView = Clipboard.GetContent();
                if (dataPackageView.Contains(StandardDataFormats.Text))
                {
                    string text = await dataPackageView.GetTextAsync();
                    mRenderer.EnqueueTask(() =>
                    {
                        mAppCore.GoToURL(text);
                    });
                }
            }, new KeyboardAccelerator() { Modifiers = VirtualKeyModifiers.Control, Key = VirtualKey.V });

            fileItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(fileItem, LocalizationHelper.Localize("Settings"), (sender, arg) =>
            {
                ShowSettings();
            });
            fileItem.Items.Add(new MenuFlyoutSeparator());

            if (!isXbox)
            {
                AppendItem(fileItem, LocalizationHelper.Localize("Open Custom Folder"), async (sender, arg) =>
                {
                    await Launcher.LaunchFolderAsync(Windows.Storage.ApplicationData.Current.LocalFolder);
                });

                fileItem.Items.Add(new MenuFlyoutSeparator());
            }

            AppendItem(fileItem, LocalizationHelper.Localize("Exit"), (sender, arg) =>
            {
                Application.Current.Exit();
            });

            void AppendCharEnterItem(MenuBarItem item, string title, short input, KeyboardAccelerator keyboardAccelerator = null)
            {
                AppendItem(item, title, (sender, arg) =>
                {
                    var flyout = (MenuFlyoutItem)sender;
                    if (!flyout.IsLoaded)
                        return;

                    mRenderer.EnqueueTask(() =>
                    {
                        mAppCore.CharEnter(input);
                    });
                }, keyboardAccelerator);
            }

            var navigationItem = CreateMenuBarItem(LocalizationHelper.Localize("Navigation"));

            AppendCharEnterItem(navigationItem, LocalizationHelper.Localize("Select Sol"), 104, new KeyboardAccelerator() { Key = VirtualKey.H });
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
                    (LocalizationHelper.Localize("Center Selection"), 99),
                    (LocalizationHelper.Localize("Go to Selection"), 103),
                    (LocalizationHelper.Localize("Follow Selection"), 102),
                    (LocalizationHelper.Localize("Sync Orbit Selection"), 121),
                    (LocalizationHelper.Localize("Track Selection"), 116)
                };
            foreach (var action in actions)
            {
                AppendCharEnterItem(navigationItem, action.Item1, action.Item2, new KeyboardAccelerator() { Key = (VirtualKey)(action.Item2 - 32) });
            }
            navigationItem.Items.Add(new MenuFlyoutSeparator());
            AppendItem(navigationItem, LocalizationHelper.Localize("Flight Mode"), (s,
                e) =>
            {
                 ShowObserverMode();
            });

            var toolsItem = CreateMenuBarItem(LocalizationHelper.Localize("Tools"));
            AppendItem(toolsItem, LocalizationHelper.Localize("Tour Guide"), (sender, arg) =>
            {
                ShowTourGuide();
            });
            AppendItem(toolsItem, LocalizationHelper.Localize("Star Browser"), (sender, arg) =>
            {
                ShowBrowser();
            });
            AppendItem(toolsItem, LocalizationHelper.Localize("Eclipse Finder"), (sender, arg) =>
            {
                ShowEclipseFinder();
            });
            toolsItem.Items.Add(new MenuFlyoutSeparator());
            AppendItem(toolsItem, LocalizationHelper.Localize("Get Add-ons"), (sender, arg) =>
            {
                ShowOnlineAddons();
            });
            AppendItem(toolsItem, LocalizationHelper.Localize("Installed Add-ons"), (sender, arg) =>
            {
                ShowAddonManagement();
            });

            var timeItem = CreateMenuBarItem(LocalizationHelper.Localize("Time"));
            AppendCharEnterItem(timeItem, LocalizationHelper.Localize("10x Faster"), 108, new KeyboardAccelerator() { Key = VirtualKey.L });
            AppendCharEnterItem(timeItem, LocalizationHelper.Localize("10x Slower"), 107, new KeyboardAccelerator() { Key = VirtualKey.K });
            AppendCharEnterItem(timeItem, LocalizationHelper.Localize("Freeze"), 32, new KeyboardAccelerator() { Key = VirtualKey.Space });
            AppendCharEnterItem(timeItem, LocalizationHelper.Localize("Real Time"), 33);
            AppendCharEnterItem(timeItem, LocalizationHelper.Localize("Reverse Time"), 106, new KeyboardAccelerator() { Key = VirtualKey.J });

            timeItem.Items.Add(new MenuFlyoutSeparator());

            AppendItem(timeItem, LocalizationHelper.Localize("Set Time…"), (sender, arg) =>
            {
                ShowTimeSetting();
            });

            var viewItem = CreateMenuBarItem(LocalizationHelper.Localize("Views"));
            AppendCharEnterItem(viewItem, LocalizationHelper.Localize("Split Horizontally"), 18, new KeyboardAccelerator() { Modifiers = VirtualKeyModifiers.Control, Key = VirtualKey.R });
            AppendCharEnterItem(viewItem, LocalizationHelper.Localize("Split Vertically"), 21, new KeyboardAccelerator() { Modifiers = VirtualKeyModifiers.Control, Key = VirtualKey.U });
            AppendCharEnterItem(viewItem, LocalizationHelper.Localize("Delete Active View"), 127, new KeyboardAccelerator() { Key = VirtualKey.Delete });
            AppendCharEnterItem(viewItem, LocalizationHelper.Localize("Delete Other Views"), 4, new KeyboardAccelerator() { Modifiers = VirtualKeyModifiers.Control, Key = VirtualKey.D });
            viewItem.Items.Add(new MenuFlyoutSeparator());
            AppendItem(viewItem, LocalizationHelper.Localize("Toggle Fullscreen"), (sender, arg) =>
            {
                ToggleFullscreen();
            }, new KeyboardAccelerator { Key = VirtualKey.F11 });

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
                mRenderer.EnqueueTask(() =>
                {
                    mAppCore.RunDemo();
                });
            });
            helpItem.Items.Add(new MenuFlyoutSeparator());
            AppendItem(helpItem, LocalizationHelper.Localize("OpenGL Info"), (sender, arg) =>
            {
                ShowOpenGLInfo();
            });
            if (!isXbox)
            {
                helpItem.Items.Add(new MenuFlyoutSeparator());
                AppendItem(helpItem, LocalizationHelper.Localize("Report a Bug"), (sender, arg) =>
                {
                    ReportBug();
                });
                AppendItem(helpItem, LocalizationHelper.Localize("Suggest a Feature"), (sender, arg) =>
                {
                    SuggestFeature();
                });
            }
            helpItem.Items.Add(new MenuFlyoutSeparator());
            AppendItem(helpItem, LocalizationHelper.Localize("Celestia Help"), (sender, arg) =>
            {
                ShowHelp();
            });
            AppendItem(helpItem, LocalizationHelper.Localize("About Celestia"), (sender, arg) =>
            {
                ShowAboutDialog();
            });

            MenuBar.Items.Add(fileItem);
            MenuBar.Items.Add(navigationItem);
            MenuBar.Items.Add(timeItem);
            MenuBar.Items.Add(toolsItem);
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

        async void ReportBug()
        {
            var tempFolder = ApplicationData.Current.TemporaryFolder;
            try
            {
                // Create all the files that might be needed in a folder to avoid collision
                var parentFolder = await tempFolder.CreateFolderAsync(GuidHelper.CreateNewGuid().ToString());
                var screenshotFile = await parentFolder.CreateFileAsync("screenshot.png");
                var renderInfoFile = await parentFolder.CreateFileAsync("renderinfo.txt");
                var urlInfoFile = await parentFolder.CreateFileAsync("urlinfo.txt");
                var systemInfoFile = await parentFolder.CreateFileAsync("systeminfo.txt");
                var addonInfoFile = await parentFolder.CreateFileAsync("addoninfo.txt");
                var crashInfoFile = await parentFolder.CreateFileAsync("crashinfo.txt");
                mRenderer.EnqueueTask(() =>
                {
                    var renderInfo = mAppCore.RenderInfo;
                    var url = mAppCore.CurrentURL;
                    bool saveScreenshotSuccess = mAppCore.SaveScreenshot(screenshotFile.Path);
                    _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                    {
                        ReportBug(saveScreenshotSuccess ? screenshotFile : null, renderInfoFile, urlInfoFile, systemInfoFile, addonInfoFile, crashInfoFile, renderInfo, url);
                    });
                });
            }
            catch
            {
                ReportBugOrSuggestFeatureFallback();
            }
        }

        async void ReportBug(StorageFile screenshotFile, StorageFile renderInfoFile, StorageFile urlInfoFile, StorageFile systemInfoFile, StorageFile addonInfoFile, StorageFile crashInfoFile, string renderInfo, string url)
        {
            try
            {
                await FileIO.WriteTextAsync(renderInfoFile, renderInfo);
                await FileIO.WriteTextAsync(urlInfoFile, url);
                var addons = await resourceManager.InstalledItems();
                var installedAddonList = "";
                foreach (var addon in addons)
                    installedAddonList += string.Format("{0}/{1}\n", addon.Name, addon.ID);
                var lastCrash = await Crashes.GetLastSessionCrashReportAsync();
                if (lastCrash != null)
                    await FileIO.WriteTextAsync(crashInfoFile, lastCrash.Id);
                await FileIO.WriteTextAsync(addonInfoFile, installedAddonList);
                var systemInfo = SystemInformation.Instance;
                var version = systemInfo.ApplicationVersion;
                var systemInfoText = string.Format(
                    "Application Version: {0}.{1}.{2}.{3}\nOperating System: {4}\nOperating System Version: {5}\nOperating System Architecture: {6}\nDevice Family: {7}\nDevice Model: {8}\nDevice Manufacturer: {9}",
                    version.Major, version.Minor, version.Build, version.Revision,
                    systemInfo.OperatingSystem,
                    systemInfo.OperatingSystemVersion,
                    systemInfo.OperatingSystemArchitectureString,
                    systemInfo.DeviceFamily,
                    systemInfo.DeviceModel,
                    systemInfo.DeviceManufacturer
                );
                await FileIO.WriteTextAsync(systemInfoFile, systemInfoText);
                var emailMessage = new EmailMessage();
                emailMessage.To.Add(new EmailRecipient(FeedbackEmailAddress));
                emailMessage.Subject = LocalizationHelper.Localize("Bug report for Celestia");
                emailMessage.Body = LocalizationHelper.Localize("Please describe the issue and repro steps, if known.");
                if (screenshotFile != null)
                    emailMessage.Attachments.Add(new EmailAttachment(screenshotFile.Name, screenshotFile));
                emailMessage.Attachments.Add(new EmailAttachment(renderInfoFile.Name, renderInfoFile));
                emailMessage.Attachments.Add(new EmailAttachment(urlInfoFile.Name, urlInfoFile));
                emailMessage.Attachments.Add(new EmailAttachment(systemInfoFile.Name, systemInfoFile));
                if (!string.IsNullOrEmpty(installedAddonList))
                    emailMessage.Attachments.Add(new EmailAttachment(addonInfoFile.Name, addonInfoFile));
                if (lastCrash != null)
                    emailMessage.Attachments.Add(new EmailAttachment(crashInfoFile.Name, crashInfoFile));
                await EmailManager.ShowComposeNewEmailAsync(emailMessage);
            }
            catch
            {
                ReportBugOrSuggestFeatureFallback();
            }
        }

        async void ReportBugOrSuggestFeatureFallback()
        {
            await Launcher.LaunchUriAsync(new Uri(FeedbackGitHubLink));
        }

        async void SuggestFeature()
        {
            try
            {
                var emailMessage = new EmailMessage();
                emailMessage.To.Add(new EmailRecipient(FeedbackEmailAddress));
                emailMessage.Subject = LocalizationHelper.Localize("Feature suggestion for Celestia");
                emailMessage.Body = LocalizationHelper.Localize("Please describe the feature you want to see in Celestia.");
                await EmailManager.ShowComposeNewEmailAsync(emailMessage);
            }
            catch
            {
                ReportBugOrSuggestFeatureFallback();
            }
        }

        void ShowHelp()
        {
            ShowPage(typeof(SafeWebPage), new Size(450, 0), GenerateWebArgsForPath("/help/welcome"));
        }

        bool ShowHelpIfNeeded()
        {
            if (!AppSettings.OnboardMessageDisplayed)
            {
                AppSettings.OnboardMessageDisplayed = true;
                AppSettings.Save(ApplicationData.Current.LocalSettings);
                ShowHelp();
                return true;
            }
            return false;
        }

        void ShowTourGuide()
        {
            ShowPage(typeof(TourGuidePage), new Size(400, 0), (mAppCore, mRenderer));
        }

        void ShowInfo(CelestiaSelection selection)
        {
            ShowPage(typeof(InfoPage), new Size(400, 0), (mAppCore, selection));
        }

        async void ShowSelectObject()
        {
            var text = await ContentDialogHelper.GetText(this, LocalizationHelper.Localize("Object name:"));
            if (text.Length > 0)
            {
                var selection = mAppCore.Simulation.Find(text);
                if (selection.IsEmpty)
                {
                    ShowObjectNotFound();
                }
                else
                {
                    mRenderer.EnqueueTask(() =>
                    {
                        mAppCore.Simulation.Selection = selection;
                    });
                }
            }
        }
        async void ShowGotoObject()
        {
            var dialog = new GotoObjectDialog(mAppCore, mRenderer);
            var result = await ContentDialogHelper.ShowContentDialogAsync(this, dialog);
            if (result == ContentDialogResult.Primary)
            {
                var objectPath = dialog.ObjectPath;
                var latitude = dialog.Latitude;
                var longitude = dialog.Longitude;
                var distance = dialog.Distance;
                var unit = (CelestiaGotoLocationDistanceUnit)dialog.Unit;
                var selection = mAppCore.Simulation.Find(objectPath);
                if (selection.IsEmpty)
                {
                    ShowObjectNotFound();
                }
                else
                {
                    var location = new CelestiaGotoLocation(selection, latitude, longitude, distance, unit);
                    mRenderer.EnqueueTask(() =>
                    {
                        mAppCore.Simulation.GoToLocation(location);
                    });
                }
            }
        }

        async void ShowObjectNotFound()
        {
            await ContentDialogHelper.ShowAlert(this, LocalizationHelper.Localize("Object not found."));
        }

        async void ShowObserverMode()
        {
            var dialog = new ObserverModeDialog(mAppCore, mRenderer);
            if (await ContentDialogHelper.ShowContentDialogAsync(this, dialog) != ContentDialogResult.Primary) return;
            var coordinateSystem = dialog.SelectedCoordinateSystem;
            var referencePath = dialog.ReferenceObjectPath;
            var targetPath = dialog.TargetObjectPath;
            mRenderer.EnqueueTask(() =>
            {
                var referenceObject = referencePath.Length == 0 ? new CelestiaSelection() : mAppCore.Simulation.Find(referencePath);
                var targetObject = targetPath.Length == 0 ? new CelestiaSelection() : mAppCore.Simulation.Find(targetPath);
                mAppCore.Simulation.ActiveObserver.SetFrame(coordinateSystem, referenceObject, targetObject);
            });
        }

        void ShowBrowser()
        {
            ShowPage(typeof(BrowserPage), new Size(500, 0), (mAppCore, mRenderer));
        }

        void ShowEclipseFinder()
        {
            ShowPage(typeof(EclipseFinderPage), new Size(400, 0), (mAppCore, mRenderer));
        }

        void ToggleFullscreen()
        {
            var view = ApplicationView.GetForCurrentView();
            if (view.IsFullScreenMode)
                view.ExitFullScreenMode();
            else
                view.TryEnterFullScreenMode();
        }

        async void ShowTimeSetting()
        {
            var dialog = new TimeSettingDialog(mAppCore.Simulation.JulianDay);
            var result = await ContentDialogHelper.ShowContentDialogAsync(this, dialog);
            if (result == ContentDialogResult.Primary)
            {
                var julianDay = dialog.JulianDay;
                mRenderer.EnqueueTask(() =>
                {
                    mAppCore.Simulation.JulianDay = julianDay;
                });
            }
        }

        void ShowSettings()
        {
            ShowPage(typeof(Settings.SettingsPage), new Size(500, 0), new SettingsArgs(mAppCore, mRenderer, AppSettings, AvailableLanguages));
        }

        void ShowPage(Type pageType, Size size, object parameter)
        {
            OverlayBackground.Visibility = Visibility.Visible;
            OverlayBackground.Width = size.Width;
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
            // Disable focus on menubar
            foreach (var item in MenuBar.Items)
            {
                item.IsTabStop = false;
            }
            if (ClosePanelButton.IsLoaded)
                ClosePanelButton.Focus(FocusState.Programmatic);
            else
                ClosePanelButton.Loaded += (s, _) =>
                {
                    ((Button)s).Focus(FocusState.Programmatic);
                };
            mRenderer.EnqueueTask(() =>
            {
                hasOverlayOpen = true;
            });
        }

        void ShowBookmarkOrganizer()
        {
            ShowPage(typeof(BookmarkOrganizerPage), new Size(400, 0), new BookmarkOrganizerParameter(mAppCore, mRenderer, false));
        }

        void ShowNewBookmark()
        {
            ShowPage(typeof(NewBookmarkPage), new Size(400, 0), new NewBookmarkParameter(mAppCore, mRenderer));
        }
        void ShowOpenGLInfo()
        {
            mRenderer.EnqueueTask(() =>
            {
                string info = mAppCore.RenderInfo;
                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
                {
                    ShowOpenGLInfo(info);
                });
            });
        }

        async void ShowOpenGLInfo(string Info)
        {
            var dialog = new InfoDialog(Info)
            {
                Title = LocalizationHelper.Localize("OpenGL Info")
            };
            await ContentDialogHelper.ShowContentDialogAsync(this, dialog);
        }

        void ShowOnlineAddons()
        {
            var queryItems = System.Web.HttpUtility.ParseQueryString("");
            queryItems.Add("lang", LocalizationHelper.Locale);
            var builder = new UriBuilder("https://celestia.mobi/resources/categories");
            builder.Query = queryItems.ToString();
            _ = Launcher.LaunchUriAsync(builder.Uri);
        }

        void ShowAddonManagement()
        {
            ShowPage(typeof(Addon.ResourceManagerPage), new Size(450, 0), new Addon.ResourceManagerPageParameter(mAppCore, mRenderer, resourceManager, delegate ()
            {
                ShowOnlineAddons();
            }));
        }

        async void ShowAboutDialog()
        {
            var dialog = new AboutDialog(PathHelper.Combine(defaultResourcePath, "AUTHORS"), PathHelper.Combine(defaultResourcePath, "TRANSLATORS"));
            await ContentDialogHelper.ShowContentDialogAsync(this, dialog);
        }

        async Task<string> GetLocale(string LocalePath)
        {
            if (AvailableLanguages == null)
            {
                var folder = await Windows.Storage.StorageFolder.GetFolderFromPathAsync(LocalePath);
                var files = await folder.GetFoldersAsync();
                var availableLocales = new List<string>();
                foreach (var file in files)
                {
                    availableLocales.Add(file.Name);
                }

                availableLocales.Sort();
                AvailableLanguages = availableLocales.ToArray();
            }
            var resourceLoader = ResourceLoader.GetForViewIndependentUse();
            return resourceLoader.GetString("CelestiaLanguage");
        }

        private async Task<JsonObject> ReadDefaultSettings()
        {
            var installedFolder = Windows.ApplicationModel.Package.Current.InstalledLocation;
            try
            {
                var defaultsFile = await installedFolder.GetFileAsync("defaults.json");
                var content = await FileIO.ReadTextAsync(defaultsFile);
                return JsonObject.Parse(content);
            }
            catch { return null; }
        }

        private void ApplySettings(JsonObject defaultSettings)
        {
            if (defaultSettings == null) return;
            var customSettings = ApplicationData.Current.LocalSettings;
            foreach (var kvp in defaultSettings)
            {
                object customSetting;
                var hasCustomSetting = customSettings.Values.TryGetValue(kvp.Key, out customSetting);
                var booleanEntry = CelestiaExtension.GetBooleanEntryByName(kvp.Key);
                if (booleanEntry != CelestiaSettingBooleanEntry.None)
                {
                    bool currentValue = false;
                    if (kvp.Value.ValueType == JsonValueType.Boolean)
                        currentValue = kvp.Value.GetBoolean();
                    else if (kvp.Value.ValueType == JsonValueType.Number)
                        currentValue = kvp.Value.GetNumber() > 0.5;
                    if (hasCustomSetting && customSetting is bool boolValue)
                        currentValue = boolValue;
                    CelestiaExtension.SetCelestiaBooleanValue(mAppCore, booleanEntry, currentValue);
                    continue;
                }
                var int32Entry = CelestiaExtension.GetInt32EntryByName(kvp.Key);
                if (int32Entry != CelestiaSettingInt32Entry.None)
                {
                    int currentValue = (int)kvp.Value.GetNumber();
                    if (hasCustomSetting && customSetting is int intValue)
                        currentValue = intValue;
                    CelestiaExtension.SetCelestiaInt32Value(mAppCore, int32Entry, currentValue);
                    continue;
                }
                var singleEntry = CelestiaExtension.GetSingleEntryByName(kvp.Key);
                if (singleEntry != CelestiaSettingSingleEntry.None)
                {
                    float currentValue = (float)kvp.Value.GetNumber();
                    if (hasCustomSetting && customSetting is float floatValue)
                        currentValue = floatValue;
                    CelestiaExtension.SetCelestiaSingleValue(mAppCore, singleEntry, currentValue);
                    continue;
                }
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
                        mRenderer.EnqueueTask(() =>
                        {
                            var selection = new CelestiaSelection(obj);
                            mAppCore.Simulation.Selection = selection;
                        });
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

        private void CaptureImage()
        {
            var tempFolder = ApplicationData.Current.TemporaryFolder;
            var path = PathHelper.Combine(tempFolder.Path, GuidHelper.CreateNewGuid().ToString() + ".png");
            mRenderer.EnqueueTask(() =>
            {
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

        private async void ShowScreenshotFailure()
        {
            await ContentDialogHelper.ShowAlert(this, LocalizationHelper.Localize("Failed in capturing screenshot."));
        }

        private async void SaveScreenshot(string path)
        {
            try
            {
                var originalFile = await StorageFile.GetFileFromPathAsync(path);
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
                StorageFile file = await savePicker.PickSaveFileAsync();
                if (file == null) return;

                await originalFile.CopyAndReplaceAsync(file);
            }
            catch
            {
                ShowScreenshotFailure();
            }
        }

        private void ClosePanelButton_Click(object sender, RoutedEventArgs e)
        {
            ClosePanel();
        }
        private void ClosePanel() {
            OverlayBackground.Visibility = Visibility.Collapsed;
            OverlayContainer.Content = null;
            // enable focus on menubar
            foreach (var item in MenuBar.Items)
            {
                item.IsTabStop = true;
            }
            // move focus to either menubar or GLView
            if (MenuBar.Visibility == Visibility.Visible)
            {
                MenuBar.Items[0].Focus(FocusState.Programmatic);
            }
            else
            {
                FocusHelperControl.Focus(FocusState.Programmatic);
            }
            mRenderer.EnqueueTask(() =>
            {
                hasOverlayOpen = false;
            });
        }
    }
}
