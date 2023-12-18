#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <fmt/format.h>
#include <fmt/printf.h>
#include <fmt/xchar.h>
#include <shlobj_core.h>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::ApplicationModel::DataTransfer;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Storage;
using namespace Windows::System;

#ifndef LOCK_CURSOR
#define LOCK_CURSOR 1
#endif

namespace winrt::CelestiaWinUI::implementation
{
    MainWindow::MainWindow()
    {
        CelestiaAppCore::SetUpLocale();
        appCore = CelestiaAppCore();

        InitializeComponent();

        Title(L"Celestia");
        WindowHelper::SetWindowIcon(*this);

        isXbox = Windows::System::Profile::AnalyticsInfo::VersionInfo().DeviceFamily() == L"Windows.Xbox";

        defaultParentPath = Windows::ApplicationModel::Package::Current().InstalledLocation().Path();
        defaultResourcePath = PathHelper::Combine(defaultParentPath, L"CelestiaResources");
        defaultConfigFilePath = PathHelper::Combine(defaultResourcePath, L"celestia.cfg");

        GLView().Loaded([weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this { weak_this.get() };
                if (strong_this)
                    strong_this->MainWindow_Loaded();
            });
    }

    fire_and_forget MainWindow::MainWindow_Loaded()
    {
        if (AppSettings().UseFullDPI())
            scale = WindowHelper::GetWindowScaleFactor(*this);

        StorageFolder customDataFolder{ nullptr };
        StorageFile customConfigFile{ nullptr };
        try
        {
            customDataFolder = co_await ApplicationData::Current().LocalFolder().GetFolderAsync(L"Override");
            customConfigFile = co_await customDataFolder.GetFileAsync(L"celestia.cfg");
        }
        catch (hresult_error const&) {}

        auto resourcePath = customDataFolder != nullptr ? customDataFolder.Path() : defaultResourcePath;
        auto configPath = customConfigFile != nullptr ? customConfigFile.Path() : defaultConfigFilePath;

        auto localePath = PathHelper::Combine(defaultResourcePath, L"locale");

        auto locale = co_await GetLocale(localePath);

        // Migrate override language to system
        auto overrideLocaleLegacy = AppSettings().LanguageOverride();
        if (!overrideLocaleLegacy.empty())
        {
            ApplicationLanguages::PrimaryLanguageOverride(LocalizationHelper::ToWindowsTag(overrideLocaleLegacy));
            AppSettings().LanguageOverride(L"");
            AppSettings().Save(ApplicationData::Current().LocalSettings());

            // We cannot change language during runtime, so still prefer this override value instead
            locale = overrideLocaleLegacy;
        }

        WindowHelper::SetWindowFlowDirection(*this);

        co_await CreateExtraFolders();
        auto defaultSettings = co_await ReadDefaultSettings();

        renderer = CelestiaRenderer(AppSettings().EnableMSAA(), [this, resourcePath, configPath, locale, defaultSettings]()
            {
                return StartEngine(resourcePath, configPath, locale, defaultSettings);
            });
        renderer.SetCorePointer(appCore.Pointer());
        renderer.SetSurface(GLView(), scale);
        renderer.Start();
    }

    AppSettings MainWindow::AppSettings()
    {
        if (appSettings == nullptr)
        {
            appSettings = CelestiaAppComponent::AppSettings(ApplicationData::Current().LocalSettings());
        }
        return appSettings;
    }

    bool MainWindow::StartEngine(hstring const& resourcePath, hstring const& configPath, hstring const& locale, JsonObject const& defaultSettings)
    {
        CelestiaAppCore::InitGL();

        std::vector<hstring> extraPaths;
        if (!extraAddonFolderPath.empty())
            extraPaths.push_back(extraAddonFolderPath);

        SetCurrentDirectoryW(resourcePath.c_str());
        hstring localeDirectory = PathHelper::Combine(resourcePath, L"locale");
        CelestiaAppCore::SetLocaleDirectory(localeDirectory, locale);

        bool loadSuccess = appCore.StartSimulation(configPath, extraPaths, [this](hstring const& status) {
            DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, status]()
                {
                    auto textToDisplay = hstring(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Loading: %s")), std::wstring(status)));
                    LoadingText().Text(textToDisplay);
                });
            });
        if (!loadSuccess)
        {
            if (resourcePath != defaultResourcePath || configPath != defaultConfigFilePath)
            {
                // Try to restore originial settings
                DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this]()
                    {
                        ContentDialogHelper::ShowAlert(Content(), LocalizationHelper::Localize(L"Error loading data, fallback to original configuration."));
                    });
                SetCurrentDirectoryW(defaultResourcePath.c_str());
                CelestiaAppCore::SetLocaleDirectory(PathHelper::Combine(defaultResourcePath, L"locale"), locale);
                if (!appCore.StartSimulation(defaultConfigFilePath, extraPaths, [this](hstring const& status) {
                    DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, status]()
                        {
                            auto textToDisplay = hstring(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Loading: %s")), std::wstring(status)));
                            LoadingText().Text(textToDisplay);
                        });
                    }))
                {
                    DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this]()
                        {
                            ShowLoadingFailure();
                        });
                    return false;
                }
            }
            else
            {
                ShowLoadingFailure();
                return false;
            }
        }

        if (!appCore.StartRenderer())
        {
            DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this]()
                {
                    ShowLoadingFailure();
                });
            return false;
        }

        LocalizationHelper::Locale(CelestiaAppCore::Language());
        UpdateScale();

        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, resourcePath]()
            {
                LoadingText().Visibility(Visibility::Collapsed);
                if (extraAddonFolder != nullptr)
                    resourceManager = ResourceManager(extraAddonFolder, extraScriptFolder);
                SetUpGLViewInteractions();
                PopulateMenuBar(resourcePath);
            });

        ApplySettings(defaultSettings);

        appCore.Start();

        readyForInput = true;

        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this]()
            {
                OpenFileOrURL();
            });
        return true;
    }

    IAsyncAction MainWindow::CreateExtraFolders()
    {
        auto folder{ ApplicationData::Current().LocalFolder() };
        try
        {
            auto mainFolder = co_await folder.CreateFolderAsync(L"CelestiaResources", CreationCollisionOption::OpenIfExists);
            extraAddonFolder = co_await mainFolder.CreateFolderAsync(L"extras", CreationCollisionOption::OpenIfExists);
            extraAddonFolderPath = extraAddonFolder.Path();
            extraScriptFolder = co_await mainFolder.CreateFolderAsync(L"scripts", CreationCollisionOption::OpenIfExists);
            extraScriptFolderPath = extraScriptFolder.Path();
        }
        catch (hresult_error const&) {}
    }

    IAsyncOperation<JsonObject> MainWindow::ReadDefaultSettings()
    {
        try
        {
            auto installedFolder{ Windows::ApplicationModel::Package::Current().InstalledLocation() };
            auto file = co_await installedFolder.GetFileAsync(L"defaults.json");
            auto text = co_await FileIO::ReadTextAsync(file);
            co_return JsonObject::Parse(text);
        }
        catch (hresult_error const&)
        {
            co_return nullptr;
        }
    }

    void MainWindow::ApplySettings(JsonObject const& defaultSettings)
    {
        if (defaultSettings == nullptr) return;
        auto localSettings = ApplicationData::Current().LocalSettings();
        for (const auto& [key, value] : defaultSettings)
        {
            auto customSetting = localSettings.Values().TryLookup(key);
            auto potentialBooleanEntry = CelestiaExtension::GetBooleanEntryByName(key);
            if (potentialBooleanEntry != CelestiaSettingBooleanEntry::None)
            {
                bool currentValue = false;
                if (value.ValueType() == JsonValueType::Boolean)
                    currentValue = value.GetBoolean();
                else
                    currentValue = value.GetNumber() > 0.5;
                if (customSetting != nullptr)
                    currentValue = unbox_value_or<bool>(customSetting, currentValue);
                CelestiaExtension::SetCelestiaBooleanValue(appCore, potentialBooleanEntry, currentValue);
                continue;
            }
            auto potentialInt32Entry = CelestiaExtension::GetInt32EntryByName(key);
            if (potentialInt32Entry != CelestiaSettingInt32Entry::None)
            {
                int32_t currentValue = (int32_t)value.GetNumber();
                if (customSetting != nullptr)
                    currentValue = unbox_value_or<int32_t>(customSetting, currentValue);
                CelestiaExtension::SetCelestiaInt32Value(appCore, potentialInt32Entry, currentValue);
                continue;
            }
            auto potentialSingleEntry = CelestiaExtension::GetSingleEntryByName(key);
            if (potentialSingleEntry != CelestiaSettingSingleEntry::None)
            {
                float currentValue = (float)value.GetNumber();
                if (customSetting != nullptr)
                    currentValue = unbox_value_or<float>(customSetting, currentValue);
                CelestiaExtension::SetCelestiaSingleValue(appCore, potentialSingleEntry, currentValue);
                continue;
            }
        }
    }

    void MainWindow::ShowLoadingFailure()
    {
        LoadingText().Text(LocalizationHelper::Localize(L"Loading Celestia failed\u2026"));
    }

    void MainWindow::UpdateScale()
    {
        appCore.SetDPI(static_cast<int>(96.0f * scale));
        appCore.PickTolerance(4.0f * static_cast<float>(AppSettings().PickSensitivity()));

        auto regularFont = L"NotoSans-Regular.ttf";
        auto regularFontIndex = 0;
        auto boldFont = L"NotoSans-Bold.ttf";
        auto boldFontIndex = 0;
        auto locale = LocalizationHelper::Locale();
        if (locale == L"ja")
        {
            regularFont = L"NotoSansCJK-Regular.ttc";
            boldFont = L"NotoSansCJK-Bold.ttc";
        }
        else if (locale == L"ko")
        {
            regularFont = L"NotoSansCJK-Regular.ttc";
            regularFontIndex = 1;
            boldFont = L"NotoSansCJK-Bold.ttc";
            boldFontIndex = 1;
        }
        else if (locale == L"zh_CN")
        {
            regularFont = L"NotoSansCJK-Regular.ttc";
            regularFontIndex = 2;
            boldFont = L"NotoSansCJK-Bold.ttc";
            boldFontIndex = 2;
        }
        else if (locale == L"zh_TW")
        {
            regularFont = L"NotoSansCJK-Regular.ttc";
            regularFontIndex = 3;
            boldFont = L"NotoSansCJK-Bold.ttc";
            boldFontIndex = 3;
        }
        else if (locale == L"ar")
        {
            regularFont = L"NotoSansArabic-Regular.ttf";
            boldFont = L"NotoSansArabic-Bold.ttf";
        }
        else if (locale == L"ka")
        {
            regularFont = L"NotoSansGeorgian-Regular.ttf";
            boldFont = L"NotoSansGeorgian-Bold.ttf";
        }

        auto textScaleFactor = Windows::UI::ViewManagement::UISettings().TextScaleFactor();
        auto pathPrefix = PathHelper::Combine(defaultResourcePath, L"fonts");
        appCore.SetHudFont(PathHelper::Combine(pathPrefix, regularFont), regularFontIndex, static_cast<int32_t>(9 * textScaleFactor));
        appCore.SetHudTitleFont(PathHelper::Combine(pathPrefix, boldFont), boldFontIndex, static_cast<int32_t>(15 * textScaleFactor));
        appCore.SetRenderFont(PathHelper::Combine(pathPrefix, regularFont), regularFontIndex, static_cast<int32_t>(9 * textScaleFactor), CelestiaFontStyle::Normal);
        appCore.SetRenderFont(PathHelper::Combine(pathPrefix, boldFont), boldFontIndex, static_cast<int32_t>(15 * textScaleFactor), CelestiaFontStyle::Large);
    }

    IAsyncOperation<hstring> MainWindow::GetLocale(hstring const& localePath)
    {
        if (availableLanguages.empty())
        {
            auto folder{ co_await StorageFolder::GetFolderFromPathAsync(localePath) };
            auto files{ co_await folder.GetFoldersAsync() };
            std::vector<hstring> availableLocales;
            for (const auto& file : files)
            {
                availableLocales.push_back(file.Name());
            }
            std::sort(availableLocales.begin(), availableLocales.end());
            availableLanguages = availableLocales;
        }
        auto resourceLoader{ Windows::ApplicationModel::Resources::ResourceLoader::GetForViewIndependentUse() };
        co_return resourceLoader.GetString(L"CelestiaLanguage");
    }

    void AppendItem(MenuFlyout const& parent, hstring const& text, RoutedEventHandler const& click, Input::KeyboardAccelerator const& accelerator = nullptr);
    void AppendItem(MenuBarItem const& parent, hstring const& text, RoutedEventHandler const& click, Input::KeyboardAccelerator const& accelerator = nullptr);
    void AppendSubItem(MenuFlyoutSubItem const& parent, hstring const& text, RoutedEventHandler click);
    void AppendToggleSubItem(MenuFlyoutSubItem const& parent, hstring const& text, bool isChecked, RoutedEventHandler const& click);
    void AppendCharEnterItem(MenuBarItem const& item, hstring const& title, short input, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, std::optional<VirtualKey> key = std::nullopt, std::optional<VirtualKeyModifiers> modifiers = std::nullopt);
    void PopulateBodyMenu(CelestiaBody const& body, MenuFlyout const& menu, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer);
    MenuFlyoutItemBase CreateMenuItem(CelestiaBrowserItem const& item, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer);

    void MainWindow::SetUpGLViewInteractions()
    {
        appCore.ShowContextMenu({ get_weak(), &MainWindow::AppCore_ShowContextMenu });
        appCore.FatalError({ get_weak(), &MainWindow::AppCore_FatalError });
        GLView().PointerPressed({ get_weak(), &MainWindow::GLView_PointerPressed });
        GLView().PointerMoved({ get_weak(), &MainWindow::GLView_PointerMoved });
        GLView().PointerReleased({ get_weak(), &MainWindow::GLView_PointerReleased });
        GLView().PointerWheelChanged({ get_weak(), &MainWindow::GLView_PointerWheelChanged });

        FocusHelperControl().Focus(FocusState::Programmatic);
        isGLViewFocused = true;
        Input::FocusManager::GotFocus({ get_weak(), &MainWindow::FocusManager_GotFocus });
        FocusHelperControl().CharacterReceived({ get_weak(), &MainWindow::FocusHelperControl_CharacterReceived });
        FocusHelperControl().KeyDown({ get_weak(), &MainWindow::FocusHelperControl_KeyDown });
        FocusHelperControl().KeyUp({ get_weak(), &MainWindow::FocusHelperControl_KeyUp });
        MenuBar().KeyUp({ get_weak() , &MainWindow::MenuBar_KeyUp });
        gamepadManager = GamepadManager(AppSettings());
        renderer.SetPreRenderTask([weak_this{ get_weak() }]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                if (!strong_this->isGLViewFocused) return;
                strong_this->gamepadManager.PollGamepad(strong_this->appCore);
            });
    }

    void MainWindow::PopulateMenuBar(hstring const& resourcePath)
    {
        MenuBar().Visibility(isXbox ? Visibility::Collapsed : Visibility::Visible);

        MenuBarItem fileItem;
        fileItem.Title(LocalizationHelper::Localize(L"File"));

        AppendItem(fileItem, LocalizationHelper::Localize(L"Run Script\u2026"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                PickScript();
            });

        MenuFlyoutSubItem scriptsItem;
        scriptsItem.Text(LocalizationHelper::Localize(L"Scripts"));

        auto scripts = CelestiaAppCore::ReadScripts(PathHelper::Combine(resourcePath, L"scripts"), true);
        for (const auto script : scripts)
        {
            AppendSubItem(scriptsItem, script.Title(), [this, script](IInspectable const&, RoutedEventArgs const&)
                {
                    appCore.RunScript(script.Filename());
                });
        }
        if (!extraScriptFolderPath.empty())
        {
            auto extraScripts = CelestiaAppCore::ReadScripts(extraScriptFolderPath, true);
            for (const auto script : extraScripts)
            {
                AppendSubItem(scriptsItem, script.Title(), [this, script](IInspectable const&, RoutedEventArgs const&)
                    {
                        appCore.RunScript(script.Filename());
                    });
            }
        }
        fileItem.Items().Append(scriptsItem);
        fileItem.Items().Append(MenuFlyoutSeparator());

        Input::KeyboardAccelerator captureImageKeyboardAccelerator;
        captureImageKeyboardAccelerator.Key(VirtualKey::F10);
        AppendItem(fileItem, LocalizationHelper::Localize(L"Capture Image"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                CaptureImage();
            }, captureImageKeyboardAccelerator);

        fileItem.Items().Append(MenuFlyoutSeparator());

        Input::KeyboardAccelerator copyKeyboardAccelerator;
        copyKeyboardAccelerator.Key(VirtualKey::C);
        copyKeyboardAccelerator.Modifiers(VirtualKeyModifiers::Control);
        AppendItem(fileItem, LocalizationHelper::Localize(L"Copy URL"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                CopyURL();
            }, copyKeyboardAccelerator);
        Input::KeyboardAccelerator pasteKeyboardAccelerator;
        pasteKeyboardAccelerator.Key(VirtualKey::V);
        pasteKeyboardAccelerator.Modifiers(VirtualKeyModifiers::Control);
        AppendItem(fileItem, LocalizationHelper::Localize(L"Paste URL"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                PasteURL();
            }, pasteKeyboardAccelerator);
        fileItem.Items().Append(MenuFlyoutSeparator());

        AppendItem(fileItem, LocalizationHelper::Localize(L"Settings"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ShowSettings();
            });
        fileItem.Items().Append(MenuFlyoutSeparator());

        if (!isXbox)
        {
            AppendItem(fileItem, LocalizationHelper::Localize(L"Open Custom Folder"), [this](IInspectable const&, RoutedEventArgs const&)
                {
                    Launcher::LaunchFolderAsync(ApplicationData::Current().LocalFolder());
                });
            fileItem.Items().Append(MenuFlyoutSeparator());
        }

        AppendItem(fileItem, LocalizationHelper::Localize(L"Exit"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                Application::Current().Exit();
            });
        MenuBarItem navigationItem;
        navigationItem.Title(LocalizationHelper::Localize(L"Navigation"));
        AppendCharEnterItem(navigationItem, LocalizationHelper::Localize(L"Select Sol"), 104, appCore, renderer, VirtualKey::H);
        AppendItem(navigationItem, LocalizationHelper::Localize(L"Select Object"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ShowSelectObject();
            });
        AppendItem(navigationItem, LocalizationHelper::Localize(L"Go to Object"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ShowGotoObject();
            });
        navigationItem.Items().Append(MenuFlyoutSeparator());


        std::vector<std::pair<hstring, int16_t>> actions =
        {
            {L"Center Selection", (int16_t)99},
            {L"Go to Selection", (int16_t)103},
            {L"Follow Selection", (int16_t)102},
            {L"Sync Orbit Selection", (int16_t)121},
            {L"Track Selection", (int16_t)116}
        };

        for (const auto& [name, code] : actions)
        {
            AppendCharEnterItem(navigationItem, LocalizationHelper::Localize(name), code, appCore, renderer, (VirtualKey)(code - 32));
        }
        navigationItem.Items().Append(MenuFlyoutSeparator());

        AppendItem(navigationItem, LocalizationHelper::Localize(L"Flight Mode"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ShowObserverMode();
            });

        MenuBarItem toolsItem;
        toolsItem.Title(LocalizationHelper::Localize(L"Tools"));
        AppendItem(toolsItem, LocalizationHelper::Localize(L"Tour Guide"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ShowTourGuide();
            });
        AppendItem(toolsItem, LocalizationHelper::Localize(L"Star Browser"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ShowBrowser();
            });
        AppendItem(toolsItem, LocalizationHelper::Localize(L"Eclipse Finder"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ShowEclipseFinder();
            });
        toolsItem.Items().Append(MenuFlyoutSeparator());
        AppendItem(toolsItem, LocalizationHelper::Localize(L"Get Add-ons"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ShowOnlineAddons();
            });
        if (!isXbox && resourceManager != nullptr)
        {
            AppendItem(toolsItem, LocalizationHelper::Localize(L"Installed Add-ons"), [this](IInspectable const&, RoutedEventArgs const&)
                {
                    ShowAddonManagement();
                });
        }

        MenuBarItem timeItem;
        timeItem.Title(LocalizationHelper::Localize(L"Time"));
        AppendCharEnterItem(timeItem, LocalizationHelper::Localize(L"10x Faster"), 108, appCore, renderer, VirtualKey::L);
        AppendCharEnterItem(timeItem, LocalizationHelper::Localize(L"10x Slower"), 107, appCore, renderer, VirtualKey::K);
        AppendCharEnterItem(timeItem, LocalizationHelper::Localize(L"Freeze"), 32, appCore, renderer, VirtualKey::Space);
        AppendCharEnterItem(timeItem, LocalizationHelper::Localize(L"Real Time"), 33, appCore, renderer);
        AppendCharEnterItem(timeItem, LocalizationHelper::Localize(L"Reverse Time"), 106, appCore, renderer, VirtualKey::J);

        timeItem.Items().Append(MenuFlyoutSeparator());

        AppendItem(timeItem, LocalizationHelper::Localize(L"Set Time\u2026"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ShowTimeSetting();
            });

        MenuBarItem viewItem;
        viewItem.Title(LocalizationHelper::Localize(L"Views"));
        AppendCharEnterItem(viewItem, LocalizationHelper::Localize(L"Split Horizontally"), 18, appCore, renderer, VirtualKey::R, VirtualKeyModifiers::Control);
        AppendCharEnterItem(viewItem, LocalizationHelper::Localize(L"Split Vertically"), 21, appCore, renderer, VirtualKey::U, VirtualKeyModifiers::Control);
        AppendCharEnterItem(viewItem, LocalizationHelper::Localize(L"Delete Active View"), 127, appCore, renderer, VirtualKey::Delete);
        AppendCharEnterItem(viewItem, LocalizationHelper::Localize(L"Delete Other Views"), 4, appCore, renderer, VirtualKey::D, VirtualKeyModifiers::Control);
        viewItem.Items().Append(MenuFlyoutSeparator());
        Input::KeyboardAccelerator fullScreenKeyboardAccelerator;
        fullScreenKeyboardAccelerator.Key(VirtualKey::F11);
        AppendItem(viewItem, LocalizationHelper::Localize(L"Toggle Fullscreen"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ToggleFullScreen();
            }, fullScreenKeyboardAccelerator);

        MenuBarItem bookmarkItem;
        bookmarkItem.Title(LocalizationHelper::Localize(L"Bookmarks"));
        AppendItem(bookmarkItem, LocalizationHelper::Localize(L"Add Bookmark"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ShowNewBookmark();
            });
        AppendItem(bookmarkItem, LocalizationHelper::Localize(L"Organize Bookmarks\u2026"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                ShowBookmarkOrganizer();
            });

        MenuBarItem helpItem;
        helpItem.Title(LocalizationHelper::Localize(L"Help"));
        AppendItem(helpItem, LocalizationHelper::Localize(L"Run Demo"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                renderer.EnqueueTask([this]()
                    {
                        appCore.RunDemo();
                    });
            });
        helpItem.Items().Append(MenuFlyoutSeparator());
        AppendItem(helpItem, LocalizationHelper::Localize(L"OpenGL Info"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                renderer.EnqueueTask([this]()
                    {
                        auto renderInfo = appCore.RenderInfo();
                        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, renderInfo]()
                            {
                                ContentDialogHelper::ShowText(Content(), LocalizationHelper::Localize(L"OpenGL Info"), renderInfo);
                            });
                    });
            });

        if (!isXbox)
        {
            helpItem.Items().Append(MenuFlyoutSeparator());
            AppendItem(helpItem, LocalizationHelper::Localize(L"Report a Bug"), [this](IInspectable const&, RoutedEventArgs const&)
                {
                    ReportBug();
                });
            AppendItem(helpItem, LocalizationHelper::Localize(L"Suggest a Feature"), [this](IInspectable const&, RoutedEventArgs const&)
                {
                    SuggestFeature();
                });
        }

        helpItem.Items().Append(MenuFlyoutSeparator());
        AppendItem(helpItem, LocalizationHelper::Localize(L"Celestia Help"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                if (isXbox)
                    ShowXboxHelp();
                else
                    ShowNonXboxHelp();
            });
        AppendItem(helpItem, LocalizationHelper::Localize(L"About Celestia"), [this](IInspectable const&, RoutedEventArgs const&)
            {
                AboutDialog dialog{ PathHelper::Combine(defaultResourcePath, L"AUTHORS"), PathHelper::Combine(defaultResourcePath, L"TRANSLATORS") };
                ContentDialogHelper::ShowContentDialogAsync(Content(), dialog);
            });

        MenuBar().Items().Append(fileItem);
        MenuBar().Items().Append(navigationItem);
        MenuBar().Items().Append(timeItem);
        MenuBar().Items().Append(toolsItem);
        MenuBar().Items().Append(viewItem);
        MenuBar().Items().Append(bookmarkItem);
        MenuBar().Items().Append(helpItem);
    }

    void MainWindow::ShowTimeSetting()
    {
        renderer.EnqueueTask([this]()
            {
                double julianDay = appCore.Simulation().JulianDay();
                DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, julianDay]()
                    {
                        ShowTimeSetting(julianDay);
                    });
            });
    }

    IAsyncAction MainWindow::ShowTimeSetting(double julianDay)
    {
        TimeSettingDialog dialog{ julianDay };
        if (co_await ContentDialogHelper::ShowContentDialogAsync(Content(), dialog) == ContentDialogResult::Primary)
        {
            auto newDay = dialog.JulianDay();
            renderer.EnqueueTask([this, newDay]()
                {
                    appCore.Simulation().JulianDay(newDay);
                });
        }
    }

    IAsyncAction MainWindow::ShowGotoObject()
    {
        GotoObjectDialog dialog{ appCore, renderer };
        if (co_await ContentDialogHelper::ShowContentDialogAsync(Content(), dialog) == ContentDialogResult::Primary)
        {
            auto objectPath = dialog.ObjectPath();
            auto latitude = dialog.Latitude();
            auto longitude = dialog.Longitude();
            auto distance = dialog.Distance();
            auto unit = dialog.UnitType();
            renderer.EnqueueTask([this, objectPath, latitude, longitude, distance, unit]
                {
                    auto selection = appCore.Simulation().Find(objectPath);
                    if (selection.IsEmpty())
                    {
                        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this]
                            {
                                ShowObjectNotFound();
                            });
                    }
                    else
                    {
                        CelestiaGotoLocation location{ selection, latitude, longitude, distance, unit };
                        appCore.Simulation().GoToLocation(location);
                    }
                });
        }
    }

    IAsyncAction MainWindow::ShowObjectNotFound()
    {
        co_await ContentDialogHelper::ShowAlert(Content(), LocalizationHelper::Localize(L"Object not found."));
    }

    IAsyncAction MainWindow::PickScript()
    {
        auto hWnd = WindowHandle();
        if (hWnd == 0) return;

        Pickers::FileOpenPicker picker;
        picker.ViewMode(Pickers::PickerViewMode::Thumbnail);
        picker.SuggestedStartLocation(Pickers::PickerLocationId::Downloads);
        picker.FileTypeFilter().Append(L".cel");
        picker.FileTypeFilter().Append(L".celx");
        auto initializeWithWindow{ picker.as<::IInitializeWithWindow>() };
        initializeWithWindow->Initialize(hWnd);
        auto file{ co_await picker.PickSingleFileAsync() };

        if (file == nullptr) co_return;

        OpenFileIfReady(file);
    }

    void MainWindow::OpenFileIfReady(StorageFile const& scriptFile)
    {
        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, scriptFile]()
            {
                scriptFileToOpen = scriptFile;
                if (readyForInput)
                    OpenFileOrURL();
            });

    }

    void MainWindow::OpenURLIfReady(Uri const& url)
    {
        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, url]()
            {
                urlToOpen = url;
                if (readyForInput)
                    OpenFileOrURL();
            });
    }

    Uri GetURIForGuide(hstring const& guide)
    {
        using namespace Windows::Web::Http;
        bool isXbox = Windows::System::Profile::AnalyticsInfo::VersionInfo().DeviceFamily() == L"Windows.Xbox";
        auto query = HttpFormUrlEncodedContent(
            {
                {L"lang", LocalizationHelper::Locale()},
                {L"guide", guide},
                {L"platform", isXbox ? L"xbox" : L"uwp"},
                {L"transparentBackground", L"1"},
            });
        return Uri(hstring(L"https://celestia.mobi/resources/guide") + L"?" + query.ToString());
    }

    Uri GetURIForPath(hstring const& path)
    {
        using namespace Windows::Web::Http;
        bool isXbox = Windows::System::Profile::AnalyticsInfo::VersionInfo().DeviceFamily() == L"Windows.Xbox";
        auto query = HttpFormUrlEncodedContent(
            {
                {L"lang", LocalizationHelper::Locale()},
                {L"platform", isXbox ? L"xbox" : L"uwp"},
                {L"transparentBackground", L"1"},
            });
        return Uri(hstring(L"https://celestia.mobi") + path + L"?" + query.ToString());
    }

    IAsyncAction MainWindow::OpenFileOrURL()
    {
        using namespace Windows::Web::Http;

        auto scriptFile = scriptFileToOpen;
        auto url = urlToOpen;
        if (scriptFile != nullptr)
        {
            if (co_await ContentDialogHelper::ShowOption(Content(), LocalizationHelper::Localize(L"Run script?")))
            {
                renderer.EnqueueTask([this, scriptFile]()
                    {
                        appCore.RunScript(scriptFile.Path());
                    });
            }
            co_return;
        }
        if (url != nullptr)
        {
            if (url.SchemeName() == L"cel")
            {
                if (co_await ContentDialogHelper::ShowOption(Content(), LocalizationHelper::Localize(L"Open URL?")))
                {
                    renderer.EnqueueTask([this, url]()
                        {
                            appCore.GoToURL(url.AbsoluteUri());
                        });
                }
                co_return;
            }

            if (!isXbox && resourceManager != nullptr && url.SchemeName() == L"celaddon" && url.Host() == L"item" && !url.Query().empty())
            {
                auto parsed = url.QueryParsed();
                if (parsed != nullptr)
                {
                    auto addon = parsed.GetFirstValueByName(L"item");
                    if (!addon.empty())
                    {
                        HttpClient client;
                        HttpFormUrlEncodedContent query({ {L"item", addon}, { L"lang", LocalizationHelper::Locale()} });
                        auto itemURL = hstring(L"https://celestia.mobi/api") + L"/resource/item" + L"?" + query.ToString();
                        try
                        {
                            auto response = co_await client.GetAsync(Uri(itemURL));
                            response.EnsureSuccessStatusCode();
                            auto content = co_await response.Content().ReadAsStringAsync();
                            auto requestResult = RequestResult::TryParse(content);
                            if (requestResult.Status() == 0)
                            {
                                auto item = ResourceItem::TryParse(requestResult.Info().Detail());
                                auto trackedWindow = WindowHelper::GetTrackedWindow(item.ID());
                                if (trackedWindow != nullptr)
                                {
                                    trackedWindow.Activate();
                                    co_return;
                                }
                                Window window;
                                window.SystemBackdrop(Media::MicaBackdrop());
                                window.Title(L"Celestia");
                                ResourceItemUserControl userControl{ appCore, renderer, item, resourceManager, [weak_window{ make_weak(window) }]()
                                    {
                                        return weak_window.get();
                                    }
                                };
                                window.Content(userControl);
                                WindowHelper::SetWindowIcon(window);
                                WindowHelper::SetWindowFlowDirection(window);
                                WindowHelper::ResizeWindow(window, 400, 600);
                                WindowHelper::TrackWindow(window, item.ID());
                                window.Activate();
                            }
                        }
                        catch (hresult_error const&) {}
                    }
                }
                co_return;
            }

            if (!isXbox && url.SchemeName() == L"celguide" && url.Host() == L"guide" && !url.Query().empty())
            {
                auto parsed = url.QueryParsed();
                if (parsed != nullptr)
                {
                    auto guide = parsed.GetFirstValueByName(L"guide");
                    if (!guide.empty())
                    {
                        auto trackedWindow = WindowHelper::GetTrackedWindow(guide);
                        if (trackedWindow != nullptr)
                        {
                            trackedWindow.Activate();
                            co_return;
                        }
                        Window window;
                        window.SystemBackdrop(Media::MicaBackdrop());
                        window.Title(L"Celestia");
                        SafeWebUserControl userControl{ CelestiaWinUI::CommonWebUserControlArgs(GetURIForGuide(guide), single_threaded_vector(std::vector<hstring>({ L"guide" })), appCore, renderer, L"", nullptr, [weak_window{ make_weak(window)}]()
                            {
                                return weak_window.get();
                            }) };
                        window.Content(userControl);
                        WindowHelper::SetWindowIcon(window);
                        WindowHelper::SetWindowFlowDirection(window);
                        WindowHelper::ResizeWindow(window, 400, 600);
                        WindowHelper::TrackWindow(window, guide);
                        window.Activate();
                    }
                }
                co_return;
            }
        }

        if (isXbox)
        {
            if (!didShowXboxWelcomeMessage && !AppSettings().IgnoreXboxWelcomeMessage())
            {
                didShowXboxWelcomeMessage = true;
                ShowXboxHelp();
            }
        }
        else
        {
            if (!AppSettings().OnboardMessageDisplayed())
            {
                AppSettings().OnboardMessageDisplayed(true);
                AppSettings().Save(ApplicationData::Current().LocalSettings());
                ShowNonXboxHelp();
                co_return;
            }

            HttpClient client;
            HttpFormUrlEncodedContent query({ {L"type", L"news"}, {L"lang", LocalizationHelper::Locale()} });
            auto latestGuideURL = hstring(L"https://celestia.mobi/api") + L"/resource/latest" + L"?" + query.ToString();
            try
            {
                auto response = co_await client.GetAsync(Uri(latestGuideURL));
                response.EnsureSuccessStatusCode();
                auto content = co_await response.Content().ReadAsStringAsync();
                auto requestResult = RequestResult::TryParse(content);
                if (requestResult.Status() == 0)
                {
                    auto item = GuideItem::TryParse(requestResult.Info().Detail());
                    if (AppSettings().LastNewsID() != item.ID())
                    {
                        auto trackedWindow = WindowHelper::GetTrackedWindow(item.ID());
                        if (trackedWindow != nullptr)
                        {
                            trackedWindow.Activate();
                            co_return;
                        }
                        Window window;
                        window.SystemBackdrop(Media::MicaBackdrop());
                        window.Title(L"Celestia");
                        SafeWebUserControl userControl{ CelestiaWinUI::CommonWebUserControlArgs(GetURIForGuide(item.ID()), single_threaded_vector(std::vector<hstring>({ L"guide" })), appCore, renderer, L"", [weak_this{ get_weak() }](hstring const& id)
                            {
                                auto strong_this = weak_this.get();
                                if (strong_this)
                                {
                                    strong_this->AppSettings().LastNewsID(id);
                                    strong_this->AppSettings().Save(ApplicationData::Current().LocalSettings());
                                }
                            }, [weak_window{ make_weak(window)}]()
                            {
                                return weak_window.get();
                            }) };
                        window.Content(userControl);
                        WindowHelper::SetWindowIcon(window);
                        WindowHelper::SetWindowFlowDirection(window);
                        WindowHelper::ResizeWindow(window, 400, 600);
                        WindowHelper::TrackWindow(window, item.ID());
                        window.Activate();
                        co_return;
                    }
                }
            }
            catch (hresult_error const&) {}
        }
    }

    void MainWindow::CopyURL()
    {
        renderer.EnqueueTask([this]()
            {
                auto url = appCore.CurrentURL();
                DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, url]()
                    {
                        DataPackage dataPackage;
                        dataPackage.RequestedOperation(DataPackageOperation::Copy);
                        dataPackage.SetText(appCore.CurrentURL());
                        Clipboard::SetContent(dataPackage);
                    });
            });
    }

    IAsyncAction MainWindow::PasteURL()
    {
        auto dataPackageView = Clipboard::GetContent();
        if (dataPackageView.Contains(StandardDataFormats::Text()))
        {
            auto text = co_await dataPackageView.GetTextAsync();
            OpenURLIfReady(Uri(text));
        }
    }

    IAsyncAction MainWindow::ShowSelectObject()
    {
        auto text = co_await ContentDialogHelper::GetText(Content(), LocalizationHelper::Localize(L"Object name:"));
        if (!text.empty())
        {
            auto selection = appCore.Simulation().Find(text);
            if (selection.IsEmpty())
            {
                ShowObjectNotFound();
            }
            else
            {
                renderer.EnqueueTask([this, selection]()
                    {
                        appCore.Simulation().Selection(selection);
                    });
            }
        }
    }

    void MainWindow::CaptureImage()
    {
        auto tempFolder{ ApplicationData::Current().TemporaryFolder() };
        auto path = PathHelper::Combine(tempFolder.Path(), to_hstring(GuidHelper::CreateNewGuid()) + L".png");
        renderer.EnqueueTask([this, path]()
            {
                int32_t oldFBO = renderer.StartReadingBackBuffer();
                bool success = appCore.SaveScreenshot(path);
                renderer.EndReadingBackBuffer(oldFBO);
                if (success)
                {
                    DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, path]()
                        {
                            SaveScreenshot(path);
                        });
                }
                else
                {
                    DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this]()
                        {
                            ContentDialogHelper::ShowAlert(Content(), LocalizationHelper::Localize(L"Failed in capturing screenshot."));
                        });
                }
            });
    }

    IAsyncAction MainWindow::SaveScreenshot(hstring const& path)
    {
        try
        {
            auto originalFile{ co_await StorageFile::GetFileFromPathAsync(path) };
            if (originalFile == nullptr) co_return;

            auto hWnd = WindowHandle();
            if (hWnd == 0) co_return;

            Pickers::FileSavePicker savePicker;
            savePicker.SuggestedStartLocation(Pickers::PickerLocationId::DocumentsLibrary);
            // Dropdown of file types the user can save the file as
            auto fileExtensions = winrt::single_threaded_observable_vector<winrt::hstring>();
            fileExtensions.Append(L".png");
            savePicker.FileTypeChoices().Insert(LocalizationHelper::Localize(L"Image"), fileExtensions);
            // Default file name if the user does not type one in or select a file to replace
            savePicker.SuggestedFileName(LocalizationHelper::Localize(L"Celestia Screenshot"));
            auto initializeWithWindow{ savePicker.as<::IInitializeWithWindow>() };
            initializeWithWindow->Initialize(hWnd);

            StorageFile file{ co_await savePicker.PickSaveFileAsync() };
            if (file == nullptr) co_return;

            co_await originalFile.CopyAndReplaceAsync(file);
        }
        catch (hresult_error const&)
        {
            ContentDialogHelper::ShowAlert(Content(), LocalizationHelper::Localize(L"Failed in capturing screenshot."));
        }
    }

    void MainWindow::ShowTourGuide()
    {
        const hstring id = L"TourGuide";
        auto trackedWindow = WindowHelper::GetTrackedWindow(id);
        if (trackedWindow != nullptr)
        {
            trackedWindow.Activate();
            return;
        }
        TourGuideWindow window{ appCore.Destinations(), appCore, renderer };
        window.SystemBackdrop(Media::MicaBackdrop());
        window.Title(LocalizationHelper::Localize(L"Tour Guide"));
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 600, 400);
        WindowHelper::TrackWindow(window, id);
        window.Activate();
    }

    void MainWindow::ShowInfo(CelestiaSelection const& selection)
    {
        // No need for tracking...
        InfoUserControl userControl{ appCore, selection };
        Window window;
        window.SystemBackdrop(Media::MicaBackdrop());
        window.Content(userControl);
        window.Title(appCore.Simulation().Universe().NameForSelection(selection));
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 400, 400);
        window.Activate();
    }

    void MainWindow::ShowEclipseFinder()
    {
        const hstring id = L"EclipseFinder";
        auto trackedWindow = WindowHelper::GetTrackedWindow(id);
        if (trackedWindow != nullptr)
        {
            trackedWindow.Activate();
            return;
        }
        EclipseFinderUserControl userControl{ appCore, renderer };
        Window window;
        window.SystemBackdrop(Media::MicaBackdrop());
        window.Title(LocalizationHelper::Localize(L"Eclipse Finder"));
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 400, 600);
        WindowHelper::TrackWindow(window, id);
        window.Activate();
    }

    fire_and_forget MainWindow::ShowObserverMode()
    {
        ObserverModeDialog dialog{ appCore, renderer };
        if (co_await ContentDialogHelper::ShowContentDialogAsync(Content(), dialog) != ContentDialogResult::Primary) co_return;
        auto coordinateSystem = dialog.SelectedCoordinateSystem();
        auto referenceObjectPath = dialog.ReferenceObjectPath();
        auto targetObjectPath = dialog.TargetObjectPath();
        renderer.EnqueueTask([this, coordinateSystem, referenceObjectPath, targetObjectPath]()
            {
                auto referenceObject = referenceObjectPath.empty() ? CelestiaSelection() : appCore.Simulation().Find(referenceObjectPath);
                auto targetObject = targetObjectPath.empty() ? CelestiaSelection() : appCore.Simulation().Find(targetObjectPath);
                appCore.Simulation().ActiveObserver().SetFrame(coordinateSystem, referenceObject, targetObject);
            });
    }

    void MainWindow::ShowBrowser()
    {
        const hstring id = L"Browser";
        auto trackedWindow = WindowHelper::GetTrackedWindow(id);
        if (trackedWindow != nullptr)
        {
            trackedWindow.Activate();
            return;
        }
        BrowserUserControl userControl{ appCore, renderer };
        Window window;
        window.SystemBackdrop(Media::MicaBackdrop());
        window.Title(LocalizationHelper::Localize(L"Star Browser"));
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 600, 400);
        WindowHelper::TrackWindow(window, id);
        window.Activate();
    }

    void MainWindow::ShowBookmarkOrganizer()
    {
        const hstring id = L"BookmarkOrganizer";
        auto trackedWindow = WindowHelper::GetTrackedWindow(id);
        if (trackedWindow != nullptr)
        {
            trackedWindow.Activate();
            return;
        }
        BookmarkOrganizerUserControl userControl{ appCore, renderer, false };
        Window window;
        window.SystemBackdrop(Media::MicaBackdrop());
        window.Title(LocalizationHelper::Localize(L"Bookmark Organizer"));
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 400, 600);
        WindowHelper::TrackWindow(window, id);
        window.Activate();
    }

    void MainWindow::ShowNewBookmark()
    {
        const hstring id = L"NewBookmark";
        auto trackedWindow = WindowHelper::GetTrackedWindow(id);
        if (trackedWindow != nullptr)
        {
            trackedWindow.Activate();
            return;
        }
        NewBookmarkUserControl userControl{ appCore, renderer };
        Window window;
        window.SystemBackdrop(Media::MicaBackdrop());
        window.Title(LocalizationHelper::Localize(L"Add Bookmark"));
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 400, 600);
        WindowHelper::TrackWindow(window, id);
        window.Activate();
    }

    void MainWindow::ShowSettings()
    {
        const hstring id = L"Settings";
        auto trackedWindow = WindowHelper::GetTrackedWindow(id);
        if (trackedWindow != nullptr)
        {
            trackedWindow.Activate();
            return;
        }
        auto languages = single_threaded_vector<hstring>();
        languages.ReplaceAll(availableLanguages);
        SettingsUserControl userControl{ appCore, renderer, AppSettings(), ApplicationData::Current().LocalSettings(), languages };
        Window window;
        window.SystemBackdrop(Media::MicaBackdrop());
        window.Title(LocalizationHelper::Localize(L"Settings"));
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 400, 600);
        WindowHelper::TrackWindow(window, id);
        window.Activate();
    }

    void MainWindow::ShowAddonManagement()
    {
        const hstring id = L"AddonManagement";
        auto trackedWindow = WindowHelper::GetTrackedWindow(id);
        if (trackedWindow != nullptr)
        {
            trackedWindow.Activate();
            return;
        }
        Window window;
        window.SystemBackdrop(Media::MicaBackdrop());
        ResourceManagerUserControl userControl{ appCore, renderer, resourceManager, [weak_window{ make_weak(window) }]()
            {
                return weak_window.get();
            },
            [weak_this{ get_weak() }]()
            {
                auto strong_this = weak_this.get();
                if (strong_this)
                    strong_this->ShowOnlineAddons();
            }
        };
        window.Content(userControl);
        window.Title(LocalizationHelper::Localize(L"Manage Add-ons"));
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 400, 600);
        WindowHelper::TrackWindow(window, id);
        window.Activate();
    }

    void MainWindow::ShowOnlineAddons()
    {
        std::map<hstring, hstring> content;
        content.emplace(L"lang", LocalizationHelper::Locale());
        auto encoder = Windows::Web::Http::HttpFormUrlEncodedContent(content);
        hstring url = hstring(L"https://celestia.mobi/resources/categories") + L"?" + encoder.ToString();
        Launcher::LaunchUriAsync(Uri(url));
    }

    void MainWindow::ToggleFullScreen()
    {
        auto appWindow = WindowHelper::GetAppWindow(*this);
        if (appWindow == nullptr) return;

        using namespace Microsoft::UI::Windowing;
        bool isFullScreen = appWindow.Presenter().Kind() == AppWindowPresenterKind::FullScreen;
        appWindow.SetPresenter(isFullScreen ? AppWindowPresenterKind::Default : AppWindowPresenterKind::FullScreen);
        isFullScreen = !isFullScreen;
        MenuBar().Visibility(isFullScreen ? Visibility::Collapsed : Visibility::Visible);
    }

    fire_and_forget MainWindow::ShowXboxHelp()
    {
        CelestiaWinUI::WelcomeDialog welcomeDialog;
        co_await ContentDialogHelper::ShowContentDialogAsync(Content(), welcomeDialog);
        if (welcomeDialog.ShouldNotShowMessageAgain())
        {
            AppSettings().IgnoreXboxWelcomeMessage(true);
            AppSettings().Save(ApplicationData::Current().LocalSettings());
        }
    }

    void MainWindow::ShowNonXboxHelp()
    {
        const hstring id = L"Welcome";
        auto trackedWindow = WindowHelper::GetTrackedWindow(id);
        if (trackedWindow != nullptr)
        {
            trackedWindow.Activate();
            return;
        }
        Window window;
        window.SystemBackdrop(Media::MicaBackdrop());
        window.Title(L"Celestia");
        SafeWebUserControl userControl{ CelestiaWinUI::CommonWebUserControlArgs(GetURIForPath(L"/help/welcome"), single_threaded_vector<hstring>(), appCore, renderer, L"", nullptr, [weak_window{make_weak(window)}]()
            {
                return weak_window.get();
            })
        };
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 400, 600);
        WindowHelper::TrackWindow(window, id);
        window.Activate();
    }

    fire_and_forget MainWindow::ReportBug()
    {
        auto tempFolder{ ApplicationData::Current().TemporaryFolder() };
        try
        {
             // Create all the files that might be needed in a folder to avoid collision
            auto parentFolder = co_await tempFolder.CreateFolderAsync(to_hstring(GuidHelper::CreateNewGuid()));
            auto screenshotFile = co_await parentFolder.CreateFileAsync(L"screenshot.png");
            auto renderInfoFile = co_await parentFolder.CreateFileAsync(L"renderinfo.txt");
            auto urlInfoFile = co_await parentFolder.CreateFileAsync(L"urlinfo.txt");
            auto systemInfoFile = co_await parentFolder.CreateFileAsync(L"systeminfo.txt");
            auto addonInfoFile = co_await parentFolder.CreateFileAsync(L"addoninfo.txt");
            hstring screenshotFilePath = screenshotFile.Path();
            renderer.EnqueueTask([this, screenshotFile, renderInfoFile, urlInfoFile, systemInfoFile, addonInfoFile, screenshotFilePath]()
                {
                    auto renderInfo = appCore.RenderInfo();
                    auto url = appCore.CurrentURL();
                    int32_t oldFBO = renderer.StartReadingBackBuffer();
                    bool saveScreenshotSuccess = appCore.SaveScreenshot(screenshotFilePath);
                    renderer.EndReadingBackBuffer(oldFBO);
                    DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, screenshotFile, renderInfoFile, urlInfoFile, systemInfoFile, addonInfoFile, renderInfo, url, saveScreenshotSuccess]()
                    {
                        ReportBug(saveScreenshotSuccess ? screenshotFile : nullptr, renderInfoFile, urlInfoFile, systemInfoFile, addonInfoFile, renderInfo, url);
                    });
                });
        }
        catch (hresult_error const&)
        {
            ReportBugOrSuggestFeatureFallback();
        }
    }

    static hstring feedbackLinkAddress = L"https://celestia.mobi/feedback";
    static hstring feedbackEmailAddress = L"celestia.mobile@outlook.com";

    IAsyncOperation<bool> SendEmail(hstring const& subject, hstring const& body, hstring const& to, Collections::IVector<StorageFile> const& files)
    {
        auto uri = EmailHelper::UriForEmail(to, subject, body, files);
        if (uri == nullptr)
            co_return false;
        try
        {
            co_await Launcher::LaunchUriAsync(uri);
            co_return true;
        }
        catch (hresult_error const&)
        {
            co_return false;
        }
    }

    fire_and_forget MainWindow::ReportBug(StorageFile const screenshotFile, StorageFile const renderInfoFile, StorageFile const urlInfoFile, StorageFile const systemInfoFile, StorageFile const addonInfoFile, hstring const renderInfo, hstring const url)
    {
        try
        {
            co_await FileIO::WriteTextAsync(renderInfoFile, renderInfo);
            co_await FileIO::WriteTextAsync(urlInfoFile, url);
            auto addons = co_await resourceManager.InstalledItems();
            std::wstring installedAddonList = L"";
            for (const auto& addon : addons)
                installedAddonList += fmt::format(L"{}/{}\n", std::wstring(addon.Name()), std::wstring(addon.ID()));

            co_await FileIO::WriteTextAsync(addonInfoFile, hstring(installedAddonList));
            auto systemInfo = SystemInformation::Instance();
            auto version = systemInfo.ApplicationVersion();
            auto systemInfoText = hstring(fmt::format(
                L"Application Version: {}.{}.{}.{}\nOperating System: {}\nOperating System Version: {}\nOperating System Architecture: {}\nDevice Family: {}\nDevice Model: {}\nDevice Manufacturer: {}",
                version.Major, version.Minor, version.Build, version.Revision,
                std::wstring(systemInfo.OperatingSystem()),
                std::wstring(systemInfo.OperatingSystemVersion()),
                std::wstring(systemInfo.OperatingSystemArchitectureString()),
                std::wstring(systemInfo.DeviceFamily()),
                std::wstring(systemInfo.DeviceModel()),
                std::wstring(systemInfo.DeviceManufacturer())
            ));
            co_await FileIO::WriteTextAsync(systemInfoFile, systemInfoText);
            Collections::IVector<StorageFile> attachments = single_threaded_vector<StorageFile>();
            attachments.Append(screenshotFile);
            attachments.Append(renderInfoFile);
            attachments.Append(urlInfoFile);
            attachments.Append(systemInfoFile);
            attachments.Append(addonInfoFile);
            if (!co_await SendEmail(LocalizationHelper::Localize(L"Bug Report for Celestia"), LocalizationHelper::Localize(L"Please describe the issue and repro steps, if known."), feedbackEmailAddress, attachments))
            {
                ReportBugOrSuggestFeatureFallback();
            }
        }
        catch (hresult_error const&)
        {
            ReportBugOrSuggestFeatureFallback();
        }
    }

    fire_and_forget MainWindow::SuggestFeature()
    {
        if (!co_await SendEmail(LocalizationHelper::Localize(L"Feature suggestion for Celestia"), LocalizationHelper::Localize(L"Please describe the feature you want to see in Celestia."), feedbackEmailAddress, single_threaded_vector<StorageFile>()))
        {
            ReportBugOrSuggestFeatureFallback();
        }
    }

    fire_and_forget MainWindow::ReportBugOrSuggestFeatureFallback()
    {
        co_await Launcher::LaunchUriAsync(Uri(feedbackLinkAddress));
    }

    HWND MainWindow::WindowHandle()
    {
        auto windowNative = try_as<::IWindowNative>();
        if (windowNative == nullptr)
            return 0;
        HWND hWnd{ 0 };
        windowNative->get_WindowHandle(&hWnd);
        return hWnd;
    }

    void MainWindow::AppCore_ShowContextMenu(IInspectable const&, ShowContextMenuArgs const& args)
    {
        auto x = args.X();
        auto y = args.Y();
        auto selection = args.Selection();
        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, selection, x, y]()
            {
                MenuFlyout menu;
                AppendItem(menu, appCore.Simulation().Universe().NameForSelection(selection), nullptr);
                menu.Items().Append(MenuFlyoutSeparator());

                AppendItem(menu, LocalizationHelper::Localize(L"Get Info"), [this, selection](IInspectable const&, RoutedEventArgs const&)
                    {
                        ShowInfo(selection);
                    });

                menu.Items().Append(MenuFlyoutSeparator());

                std::vector<std::pair<hstring, int16_t>> actions = { {L"Go", (int16_t)103}, {L"Follow", (int16_t)102}, {L"Sync Orbit", (int16_t)121} };
                for (const auto& [name, code] : actions)
                {
                    auto copiedCode = code;
                    AppendItem(menu, LocalizationHelper::Localize(name), [this, selection, copiedCode](IInspectable const&, RoutedEventArgs const&)
                        {
                            renderer.EnqueueTask([this, selection, copiedCode]()
                                {
                                    appCore.Simulation().Selection(selection);
                                    appCore.CharEnter(copiedCode);
                                });
                        });
                }
                CelestiaBody body = selection.Object().try_as<CelestiaBody>();
                if (body != nullptr)
                {
                    PopulateBodyMenu(body, menu, appCore, renderer);
                }

                std::vector<MenuFlyoutItemBase> browserMenuItems;
                CelestiaBrowserItem browserItem = { appCore.Simulation().Universe().NameForSelection(selection), selection.Object(), [this](CelestiaBrowserItem const& item) { return CelestiaExtension::GetChildren(item, appCore); }, false };
                if (!browserItem.Children().Size() == 0)
                {
                    for (const auto& child : browserItem.Children())
                    {
                        browserMenuItems.push_back(CreateMenuItem(child, appCore, renderer));
                    }
                }

                if (!browserMenuItems.empty())
                {
                    menu.Items().Append(MenuFlyoutSeparator());
                    for (const auto& browserMenuItem : browserMenuItems)
                    {
                        menu.Items().Append(browserMenuItem);
                    }
                }
                menu.Items().Append(MenuFlyoutSeparator());

                if (appCore.Simulation().Universe().IsSelectionMarked(selection))
                {
                    AppendItem(menu, LocalizationHelper::Localize(L"Unmark"), [this, selection](IInspectable const&, RoutedEventArgs const&)
                        {
                            renderer.EnqueueTask([this, selection]()
                                {
                                    appCore.Simulation().Universe().UnmarkSelection(selection);
                                });
                        });
                }
                else
                {
                    MenuFlyoutSubItem action;
                    action.Text(LocalizationHelper::Localize(L"Mark"));
                    std::vector<std::pair<hstring, CelestiaMarkerRepresentation>> markers =
                    {
                        {L"Diamond", CelestiaMarkerRepresentation::Diamond},
                        {L"Triangle", CelestiaMarkerRepresentation::Triangle},
                        {L"Filled Square", CelestiaMarkerRepresentation::FilledSquare},
                        {L"Plus", CelestiaMarkerRepresentation::Plus},
                        {L"X", CelestiaMarkerRepresentation::X},
                        {L"Left Arrow", CelestiaMarkerRepresentation::LeftArrow},
                        {L"Right Arrow", CelestiaMarkerRepresentation::RightArrow},
                        {L"Up Arrow", CelestiaMarkerRepresentation::UpArrow},
                        {L"Down Arrow", CelestiaMarkerRepresentation::DownArrow},
                        {L"Circle", CelestiaMarkerRepresentation::Circle},
                        {L"Disk", CelestiaMarkerRepresentation::Disk},
                        {L"Crosshair", CelestiaMarkerRepresentation::Crosshair}
                    };

                    for (const auto& [name, type] : markers)
                    {
                        auto copiedType = type;
                        AppendSubItem(action, LocalizationHelper::Localize(name), [this, selection, copiedType](IInspectable const&, RoutedEventArgs const&)
                            {
                                renderer.EnqueueTask([this, selection, copiedType]()
                                    {
                                        appCore.Simulation().Universe().MarkSelection(selection, copiedType);
                                        appCore.ShowMarkers(true);
                                    });
                            });
                    }
                    menu.Items().Append(action);
                }

                menu.ShowAt(GLView(), Point(x / scale, y / scale));
            });
    }

    void MainWindow::AppCore_FatalError(IInspectable const&, FatalErrorArgs const& args)
    {
        auto message = args.Message();
        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [weak_this{ get_weak() }, message]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this != nullptr)
                {
                    ContentDialogHelper::ShowAlert(strong_this->Content(), message);
                }
            });
    }

    void MainWindow::GLView_PointerPressed(IInspectable const& sender, Input::PointerRoutedEventArgs const& args)
    {
        if (args.Pointer().PointerDeviceType() == Microsoft::UI::Input::PointerDeviceType::Mouse)
        {
            auto properties{ args.GetCurrentPoint(sender.as<UIElement>()).Properties() };
            std::optional<CelestiaMouseButton> newButtonPressed = std::nullopt;
            if (properties.IsLeftButtonPressed() && currentPressedButton != CelestiaMouseButton::Left)
            {
                newButtonPressed = CelestiaMouseButton::Left;
            }
            else if (properties.IsRightButtonPressed() && currentPressedButton != CelestiaMouseButton::Right)
            {
                newButtonPressed = CelestiaMouseButton::Right;
            }
            else if (properties.IsMiddleButtonPressed() && currentPressedButton != CelestiaMouseButton::Middle)
            {
                newButtonPressed = CelestiaMouseButton::Middle;
            }
            if (newButtonPressed.has_value())
            {
                auto currentButton = currentPressedButton;
                currentPressedButton = newButtonPressed;
                auto position{ args.GetCurrentPoint(sender.as<UIElement>()).Position() };
                auto scaledPosition = Point(position.X * scale, position.Y * scale);

                renderer.EnqueueTask([this, scaledPosition, currentButton, newButtonPressed]()
                    {
                        if (currentButton.has_value())
                            appCore.MouseButtonUp(static_cast<float>(scaledPosition.X), static_cast<float>(scaledPosition.Y), currentButton.value());
                        appCore.MouseButtonDown(static_cast<float>(scaledPosition.X), static_cast<float>(scaledPosition.Y), newButtonPressed.value());
                    });

                lastMousePosition = position;
#if LOCK_CURSOR
                POINT globalPosition;
                if (GetCursorPos(&globalPosition))
                    lastMouseGlobalPosition = Point(static_cast<float>(globalPosition.x), static_cast<float>(globalPosition.y));
                else
                    lastMouseGlobalPosition = std::nullopt;
#endif

                if (!isMouseHidden)
                {
                    GLView().ProtectedCursor(Microsoft::UI::Input::InputSystemCursor::Create(Microsoft::UI::Input::InputSystemCursorShape::Arrow));
                    GLView().ProtectedCursor().Close();
                    GLView().CapturePointer(args.Pointer());
                    isMouseHidden = true;
                }
            }
        }
    }

    void MainWindow::GLView_PointerMoved(IInspectable const& sender, Input::PointerRoutedEventArgs const& args)
    {
        if (args.Pointer().PointerDeviceType() == Microsoft::UI::Input::PointerDeviceType::Mouse)
        {
            if (!lastMousePosition.has_value() || !currentPressedButton.has_value()) return;

            auto properties = args.GetCurrentPoint(sender.as<UIElement>()).Properties();
            auto position = args.GetCurrentPoint(sender.as<UIElement>()).Position();
            auto scaledPosition = Point(position.X * scale, position.Y * scale);
            auto oldPosition = lastMousePosition.value();
            auto oldScaledPosition = Point(oldPosition.X * scale, oldPosition.Y * scale);
            auto x = scaledPosition.X - oldScaledPosition.X;
            auto y = scaledPosition.Y - oldScaledPosition.Y;
            auto button = currentPressedButton.value();

            renderer.EnqueueTask([this, x, y, button]()
                {
                    appCore.MouseMove((float)x, (float)y, button);
                });

#if LOCK_CURSOR
            if (lastMouseGlobalPosition.has_value())
            {
                int globalX = static_cast<int>(lastMouseGlobalPosition.value().X);
                int globalY = static_cast<int>(lastMouseGlobalPosition.value().Y);
                if (!SetCursorPos(globalX, globalY))
                {
                    lastMouseGlobalPosition = std::nullopt;
                    lastMousePosition = position;
                }
            }
            else
            {
                lastMousePosition = position;
            }
#else
            lastMousePosition = position;
#endif
        }
    }

    void MainWindow::GLView_PointerReleased(IInspectable const& sender, Input::PointerRoutedEventArgs const& args)
    {
        if (args.Pointer().PointerDeviceType() == Microsoft::UI::Input::PointerDeviceType::Mouse)
        {
            if (!currentPressedButton.has_value()) return;
            auto properties = args.GetCurrentPoint(sender.as<UIElement>()).Properties();
            bool buttonReleased = false;
            if (currentPressedButton == CelestiaMouseButton::Left)
            {
                buttonReleased = !properties.IsLeftButtonPressed();
            }
            else if (currentPressedButton == CelestiaMouseButton::Right)
            {
                buttonReleased = !properties.IsRightButtonPressed();
            }
            else if (currentPressedButton == CelestiaMouseButton::Middle)
            {
                buttonReleased = !properties.IsMiddleButtonPressed();
            }

            if (buttonReleased)
            {
                auto button = currentPressedButton.value();
                auto position = args.GetCurrentPoint(sender.as<UIElement>()).Position();
                auto scaledPosition = Point(position.X * scale, position.Y * scale);
                renderer.EnqueueTask([this, scaledPosition, button]()
                    {
                        appCore.MouseButtonUp(static_cast<float>(scaledPosition.X), static_cast<float>(scaledPosition.Y), button);
                    });
                lastMousePosition = std::nullopt;
                currentPressedButton = std::nullopt;
#if LOCK_CURSOR
                lastMouseGlobalPosition = std::nullopt;
#endif

                if (isMouseHidden)
                {
                    GLView().ProtectedCursor(Microsoft::UI::Input::InputSystemCursor::Create(Microsoft::UI::Input::InputSystemCursorShape::Arrow));
                    GLView().ReleasePointerCapture(args.Pointer());
                    isMouseHidden = false;
                }
            }
        }
    }

    void MainWindow::GLView_PointerWheelChanged(IInspectable const& sender, Input::PointerRoutedEventArgs const& args)
    {
        auto delta = args.GetCurrentPoint(sender.as<UIElement>()).Properties().MouseWheelDelta();
        renderer.EnqueueTask([this, delta]()
            {
                appCore.MouseWheel(delta > 0 ? -1.0f : 1.0f, 0);
            });
    }

    void MainWindow::FocusManager_GotFocus(IInspectable const&, Input::FocusManagerGotFocusEventArgs const& args)
    {
        auto focusedItem = args.NewFocusedElement();
        auto glViewHasFocus = focusedItem == FocusHelperControl();
        if (!glViewHasFocus)
        {
            auto element = focusedItem.try_as<FrameworkElement>();
            if (element != nullptr && element.XamlRoot() == Content().XamlRoot())
            {
                // Do not give up focus to GLView and its parents
                FrameworkElement parent = GLView();
                while (parent != nullptr && parent != element)
                {
                    auto potentialParent = parent.Parent();
                    if (potentialParent != nullptr)
                        parent = potentialParent.try_as<FrameworkElement>();
                    else
                        parent = nullptr;
                }
                if (parent == element)
                {
                    FocusHelperControl().Focus(FocusState::Programmatic);
                    glViewHasFocus = true;
                }
            }
        }

        renderer.EnqueueTask([this, glViewHasFocus]()
            {
                isGLViewFocused = glViewHasFocus;
            });
    }

    void MainWindow::FocusHelperControl_CharacterReceived(IInspectable const&, Input::CharacterReceivedRoutedEventArgs const& args)
    {
        int16_t key = args.Character();
        renderer.EnqueueTask([this, key]()
            {
                appCore.CharEnter(key);
            });
    }

    void MainWindow::FocusHelperControl_KeyDown(IInspectable const&, Input::KeyRoutedEventArgs const& args)
    {
        if (args.OriginalKey() >= VirtualKey::GamepadA && args.OriginalKey() <= VirtualKey::GamepadRightThumbstickLeft) return;

        int32_t modifiers = 0;
        auto key = args.Key();

        bool isShiftPressed = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        bool isControlPressed = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        if (isControlPressed)
            modifiers |= 16;
        if (isShiftPressed)
            modifiers |= 8;

        renderer.EnqueueTask([this, key, modifiers]()
            {
                appCore.KeyDown((int32_t)key, modifiers);
            });
    }

    void MainWindow::MenuBar_KeyUp(IInspectable const&, Input::KeyRoutedEventArgs const& args)
    {
        if (isXbox && args.Key() == VirtualKey::GamepadMenu)
        {
            MenuBar().Visibility(Visibility::Collapsed);
            FocusHelperControl().Focus(FocusState::Programmatic);
        }
    }

    void MainWindow::FocusHelperControl_KeyUp(IInspectable const&, Input::KeyRoutedEventArgs const& args)
    {
        if (isXbox)
        {
            if (args.Key() == VirtualKey::GamepadMenu)
            {
                MenuBar().Visibility(Visibility::Visible);
                if (MenuBar().Items().GetAt(0).IsLoaded())
                    MenuBar().Items().GetAt(0).Focus(FocusState::Programmatic);
                else
                    MenuBar().Items().GetAt(0).Loaded([](IInspectable const& sender, RoutedEventArgs const&)
                        {
                            sender.as<MenuBarItem>().Focus(FocusState::Programmatic);
                        });
                return;
            }
        }

        if (args.OriginalKey() >= VirtualKey::GamepadA && args.OriginalKey() <= VirtualKey::GamepadRightThumbstickLeft) return;

        int key = (int32_t)args.Key();

        renderer.EnqueueTask([this, key]()
            {
                appCore.KeyUp(key, 0);
            });
    }

    void AppendItem(MenuFlyout const& parent, hstring const& text, RoutedEventHandler const& click, Input::KeyboardAccelerator const& accelerator)
    {
        MenuFlyoutItem item;
        item.Text(text);
        if (accelerator != nullptr)
            item.KeyboardAccelerators().Append(accelerator);
        if (click != nullptr)
            item.Click(click);
        parent.Items().Append(item);
    }

    void AppendItem(MenuBarItem const& parent, hstring const& text, RoutedEventHandler const& click, Input::KeyboardAccelerator const& accelerator)
    {
        MenuFlyoutItem item;
        item.Text(text);
        if (accelerator != nullptr)
            item.KeyboardAccelerators().Append(accelerator);
        if (click != nullptr)
            item.Click(click);
        parent.Items().Append(item);
    }

    void AppendSubItem(MenuFlyoutSubItem const& parent, hstring const& text, RoutedEventHandler click)
    {
        MenuFlyoutItem item;
        item.Text(text);
        if (click != nullptr)
            item.Click(click);
        parent.Items().Append(item);
    }

    void AppendToggleSubItem(MenuFlyoutSubItem const& parent, hstring const& text, bool isChecked, RoutedEventHandler const& click)
    {
        ToggleMenuFlyoutItem item;
        item.IsChecked(isChecked);
        item.Text(text);
        item.Click(click);
        parent.Items().Append(item);
    }

    void AppendCharEnterItem(MenuBarItem const& item, hstring const& title, short input, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, std::optional<VirtualKey> key, std::optional<VirtualKeyModifiers> modifiers)
    {
        Input::KeyboardAccelerator keyboardAccelerator = nullptr;
        if (key.has_value())
        {
            keyboardAccelerator = Input::KeyboardAccelerator();
            keyboardAccelerator.Key(key.value());
            if (modifiers.has_value())
            {
                keyboardAccelerator.Modifiers(modifiers.value());
            }
        }
        AppendItem(item, title, [appCore, renderer, input](IInspectable const& sender, RoutedEventArgs const&)
            {
                auto flyoutItem = sender.as<MenuFlyoutItem>();
                if (!flyoutItem.IsLoaded())
                    return;
                renderer.EnqueueTask([appCore, input]
                    {
                        appCore.CharEnter(input);
                    });
            }, keyboardAccelerator);
    }

    void PopulateBodyMenu(CelestiaBody const& body, MenuFlyout const& menu, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer)
    {
        auto surfaces = body.AlternateSurfaceNames();
        if (surfaces.size() > 0)
        {
            menu.Items().Append(MenuFlyoutSeparator());

            MenuFlyoutSubItem altSur;
            altSur.Text(LocalizationHelper::Localize(L"Alternate Surfaces"));
            AppendSubItem(altSur, LocalizationHelper::Localize(L"Default"), [appCore, renderer](IInspectable const&, RoutedEventArgs const&)
                {
                    renderer.EnqueueTask([appCore]()
                        {
                            appCore.Simulation().ActiveObserver().DisplayedSurfaceName(L"");
                        });
                });

            for (const auto name : surfaces)
            {
                AppendSubItem(altSur, name, [appCore, renderer, name](IInspectable const&, RoutedEventArgs const&)
                    {
                        renderer.EnqueueTask([appCore, name]()
                            {
                                appCore.Simulation().ActiveObserver().DisplayedSurfaceName(name);
                            });
                    });
            }

            menu.Items().Append(altSur);
        }

        menu.Items().Append(MenuFlyoutSeparator());
        MenuFlyoutSubItem refMarkMenu;
        refMarkMenu.Text(LocalizationHelper::Localize(L"Reference Vectors"));


        enum class RefMarkMenuActionType
        {
            ShowBodyAxes, ShowFrameAxes, ShowSunDirection, ShowVelocityVector, ShowPlanetographicGrid, ShowTerminator
        };

        std::vector<std::pair<hstring, RefMarkMenuActionType>> refMarkMenuActions =
        {
            {L"Show Body Axes", RefMarkMenuActionType::ShowBodyAxes},
            {L"Show Frame Axes", RefMarkMenuActionType::ShowFrameAxes},
            {L"Show Sun Direction", RefMarkMenuActionType::ShowSunDirection},
            {L"Show Velocity Vector", RefMarkMenuActionType::ShowVelocityVector},
            {L"Show Planetographic Grid", RefMarkMenuActionType::ShowPlanetographicGrid},
            {L"Show Terminator", RefMarkMenuActionType::ShowTerminator}
        };
        for (const auto& [name, type] : refMarkMenuActions)
        {
            bool isEnabled = false;
            switch (type)
            {
            case RefMarkMenuActionType::ShowBodyAxes:
                isEnabled = appCore.ShowBodyAxes();
                break;
            case RefMarkMenuActionType::ShowFrameAxes:
                isEnabled = appCore.ShowFrameAxes();
                break;
            case RefMarkMenuActionType::ShowSunDirection:
                isEnabled = appCore.ShowSunDirection();
                break;
            case RefMarkMenuActionType::ShowVelocityVector:
                isEnabled = appCore.ShowVelocityVector();
                break;
            case RefMarkMenuActionType::ShowPlanetographicGrid:
                isEnabled = appCore.ShowPlanetographicGrid();
                break;
            case RefMarkMenuActionType::ShowTerminator:
                isEnabled = appCore.ShowTerminator();
                break;
            default:
                break;
            }
            auto copiedType = type;
            AppendToggleSubItem(refMarkMenu, LocalizationHelper::Localize(name), isEnabled, [appCore, renderer, copiedType, isEnabled](IInspectable const&, RoutedEventArgs const&)
                {
                    renderer.EnqueueTask([appCore, copiedType, isEnabled]()
                        {
                            switch (copiedType)
                            {
                            case RefMarkMenuActionType::ShowBodyAxes:
                                appCore.ShowBodyAxes(!isEnabled);
                                break;
                            case RefMarkMenuActionType::ShowFrameAxes:
                                appCore.ShowFrameAxes(!isEnabled);
                                break;
                            case RefMarkMenuActionType::ShowSunDirection:
                                appCore.ShowSunDirection(!isEnabled);
                                break;
                            case RefMarkMenuActionType::ShowVelocityVector:
                                appCore.ShowVelocityVector(!isEnabled);
                                break;
                            case RefMarkMenuActionType::ShowPlanetographicGrid:
                                appCore.ShowPlanetographicGrid(!isEnabled);
                                break;
                            case RefMarkMenuActionType::ShowTerminator:
                                appCore.ShowTerminator(!isEnabled);
                                break;
                            default:
                                break;
                            }
                        });
                });
        }
        menu.Items().Append(refMarkMenu);
    }

    MenuFlyoutItemBase CreateMenuItem(CelestiaBrowserItem const& item, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer)
    {
        MenuFlyoutSubItem menu;
        menu.Text(item.Name());

        menu.Loaded([appCore, renderer, item](IInspectable const& sender, RoutedEventArgs const&)
            {
                auto senderMenu = sender.as<MenuFlyoutSubItem>();
                if (senderMenu.Items().Size() > 0) return;

                std::vector<MenuFlyoutItemBase> children;
                auto obj = item.Object();
                if (obj != nullptr)
                {
                    MenuFlyoutItem selectItem;
                    selectItem.Text(LocalizationHelper::Localize(L"Select"));
                    selectItem.Click([appCore, renderer, obj](IInspectable const&, RoutedEventArgs const&)
                        {
                            renderer.EnqueueTask([appCore, obj]()
                                {
                                    CelestiaSelection selection{ obj };
                                    appCore.Simulation().Selection(selection);
                                });
                        });
                    children.push_back(selectItem);
                }
                if (!item.Children().Size() == 0)
                {
                    children.push_back(MenuFlyoutSeparator());

                    for (const auto& child : item.Children())
                    {
                        children.push_back(CreateMenuItem(child, appCore, renderer));
                    }
                }
                for (const auto& child : children)
                {
                    senderMenu.Items().Append(child);
                }
            });
        return menu;
    }
}