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
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Storage;
using namespace Windows::System;
using namespace Windows::UI::ViewManagement;

#ifndef LOCK_CURSOR
#define LOCK_CURSOR 1
#endif

namespace winrt::CelestiaWinUI::implementation
{
    MainWindow::MainWindow()
    {
        CelestiaAppCore::SetUpLocale();
        appCore = CelestiaAppCore();
        appSettings = CelestiaAppComponent::AppSettings(ApplicationData::Current().LocalSettings());

        isXbox = Windows::System::Profile::AnalyticsInfo::VersionInfo().DeviceFamily() == L"Windows.Xbox";

        defaultParentPath = Windows::ApplicationModel::Package::Current().InstalledLocation().Path();
        defaultResourcePath = PathHelper::Combine(defaultParentPath, L"CelestiaResources");
        defaultConfigFilePath = PathHelper::Combine(defaultResourcePath, L"celestia.cfg");
    }

    void MainWindow::InitializeComponent()
    {
        MainWindowT::InitializeComponent();

        Title(L"Celestia");
        WindowHelper::SetWindowIcon(*this);
        WindowHelper::SetWindowTheme(*this);

        GLView().Loaded([weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this { weak_this.get() };
                if (strong_this)
                    strong_this->MainWindow_Loaded();
            });

        Closed([weak_this{ get_weak() }](auto&&, auto&&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this)
                    strong_this->isClosed = true;
            });
    }

    void MainWindow::WillActivate(Windows::ApplicationModel::Activation::IActivatedEventArgs const& args)
    {
        using namespace Windows::ApplicationModel::Activation;

        if (args == nullptr) return;

        if (args.Kind() == ActivationKind::File)
        {
            auto fileArgs = args.as<FileActivatedEventArgs>();
            auto file = fileArgs.Files().GetAt(0).as<Windows::Storage::StorageFile>();
            OpenFileIfReady(file);
        }
        else if (args.Kind() == ActivationKind::Protocol)
        {
            auto protocolArgs = args.as<ProtocolActivatedEventArgs>();
            OpenURLIfReady(protocolArgs.Uri());
        }
    }

    fire_and_forget MainWindow::MainWindow_Loaded()
    {
        auto strong_this{ get_strong() };
        if (appSettings.UseFullDPI())
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
        auto overrideLocaleLegacy = appSettings.LanguageOverride();
        if (!overrideLocaleLegacy.empty())
        {
            Windows::Globalization::ApplicationLanguages::PrimaryLanguageOverride(LocalizationHelper::ToWindowsTag(overrideLocaleLegacy));
            appSettings.LanguageOverride(L"");
            appSettings.Save(ApplicationData::Current().LocalSettings());

            // We cannot change language during runtime, so still prefer this override value instead
            locale = overrideLocaleLegacy;
        }

        WindowHelper::SetWindowFlowDirection(*this);

        auto resourceLoader{ Windows::ApplicationModel::Resources::ResourceLoader::GetForViewIndependentUse() };
        CelestiaLayoutDirection layoutDirection = resourceLoader.GetString(L"ApplicationFlowDirection") == L"RightToLeft" ? CelestiaLayoutDirection::RTL : CelestiaLayoutDirection::LTR;

        co_await CreateExtraFolders();
        auto defaultSettings = co_await ReadDefaultSettings();

        renderer = CelestiaRenderer(appSettings.EnableMSAA(), [weak_this{ get_weak() }, resourcePath, configPath, locale, layoutDirection, defaultSettings](int32_t)
            {
                auto strong_this{ weak_this.get() };
                return strong_this == nullptr ? false : strong_this->StartEngine(resourcePath, configPath, locale, layoutDirection, defaultSettings);
            });
        renderer.SetCorePointer(appCore.Pointer());
        renderer.SetSurface(GLView(), scale);
        co_await renderer.Start();
    }

    bool MainWindow::StartEngine(hstring const resourcePath, hstring const& configPath, hstring const& locale, CelestiaLayoutDirection layoutDirection, JsonObject const& defaultSettings)
    {
        CelestiaAppCore::InitGL();

        std::vector<hstring> extraPaths;
        if (!extraAddonFolderPath.empty())
            extraPaths.push_back(extraAddonFolderPath);

        SetCurrentDirectoryW(resourcePath.c_str());
        hstring localeDirectory = PathHelper::Combine(resourcePath, L"locale");
        CelestiaAppCore::SetLocaleDirectory(localeDirectory, locale);

        bool loadSuccess = appCore.StartSimulation(configPath, extraPaths, [this](hstring const status) {
            DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, status]()
                {
                    auto textToDisplay = hstring(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Loading: %s", L"Celestia initialization, loading file")), std::wstring(status)));
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
                        ContentDialogHelper::ShowAlert(Content(), LocalizationHelper::Localize(L"Error loading data, fallback to original configuration.", L""));
                    });
                SetCurrentDirectoryW(defaultResourcePath.c_str());
                CelestiaAppCore::SetLocaleDirectory(PathHelper::Combine(defaultResourcePath, L"locale"), locale);
                if (!appCore.StartSimulation(defaultConfigFilePath, extraPaths, [this](hstring const status) {
                    DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, status]()
                        {
                            auto textToDisplay = hstring(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Loading: %s", L"Celestia initialization, loading file")), std::wstring(status)));
                            LoadingText().Text(textToDisplay);
                        });
                    }))
                {
                    ShowLoadingFailure();
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
            ShowLoadingFailure();
            return false;
        }

        LocalizationHelper::Locale(CelestiaAppCore::Language());
        appCore.LayoutDirection(layoutDirection);
        UpdateScale();

        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [weak_this{ get_weak() }, resourcePath]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->LoadingText().Visibility(Visibility::Collapsed);
                strong_this->resourceManager = ResourceManager(strong_this->extraAddonFolder, strong_this->extraScriptFolder);
                strong_this->SetUpGLViewInteractions();
                strong_this->PopulateMenuBar(resourcePath);
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
        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this]()
            {
                LoadingText().Text(LocalizationHelper::Localize(L"Loading Celestia failed\u2026", L"Celestia loading failed"));
            });
    }

    void MainWindow::UpdateScale()
    {
        appCore.SetDPI(static_cast<int>(96.0f * scale));
        appCore.PickTolerance(scale * static_cast<float>(appSettings.PickSensitivity()));

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
        try
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
        catch (hresult_error const&)
        {
            co_return L"en";
        }
    }

    void AppendItem(MenuFlyout const& parent, hstring const& text, RoutedEventHandler const& click, Input::KeyboardAccelerator const& accelerator = nullptr);
    void AppendItem(MenuBarItem const& parent, hstring const& text, RoutedEventHandler const& click, Input::KeyboardAccelerator const& accelerator = nullptr);
    void AppendSubItem(MenuFlyoutSubItem const& parent, hstring const& text, RoutedEventHandler click);
    void AppendToggleSubItem(MenuFlyoutSubItem const& parent, hstring const& text, bool isChecked, RoutedEventHandler const& click);
    void AppendCharEnterItem(MenuBarItem const& item, hstring const& title, short input, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, std::optional<VirtualKey> key = std::nullopt, std::optional<VirtualKeyModifiers> modifiers = std::nullopt);
    void PopulateBodyMenu(CelestiaBody const& body, MenuFlyout const& menu, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer);
    MenuFlyoutItemBase CreateMenuItem(CelestiaBrowserItem const item, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer);

    void MainWindow::SetUpGLViewInteractions()
    {
        appCore.ShowContextMenu({ get_weak(), &MainWindow::AppCore_ShowContextMenu });
        appCore.FatalError({ get_weak(), &MainWindow::AppCore_FatalError });
        appCore.SystemAccessRequest({ get_weak(), &MainWindow::AppCore_SystemAccessRequest });
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
        gamepadManager = GamepadManager(appSettings);
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
        fileItem.Title(LocalizationHelper::Localize(L"File", L"File menu"));

        AppendItem(fileItem, LocalizationHelper::Localize(L"Run Script\u2026", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->PickScript();
            });

        MenuFlyoutSubItem scriptsItem;
        scriptsItem.Text(LocalizationHelper::Localize(L"Scripts", L""));

        auto scripts = CelestiaAppCore::ReadScripts(PathHelper::Combine(resourcePath, L"scripts"), true);
        for (const auto script : scripts)
        {
            AppendSubItem(scriptsItem, script.Title(), [weak_this{ get_weak() }, script](IInspectable const&, RoutedEventArgs const&)
                {
                    auto strong_this{ weak_this.get() };
                    if (strong_this == nullptr) return;
                    strong_this->appCore.RunScript(script.Filename());
                });
        }
        if (!extraScriptFolderPath.empty())
        {
            auto extraScripts = CelestiaAppCore::ReadScripts(extraScriptFolderPath, true);
            for (const auto script : extraScripts)
            {
                AppendSubItem(scriptsItem, script.Title(), [weak_this{ get_weak() }, script](IInspectable const&, RoutedEventArgs const&)
                    {
                        auto strong_this{ weak_this.get() };
                        if (strong_this == nullptr) return;
                        strong_this->appCore.RunScript(script.Filename());
                    });
            }
        }
        fileItem.Items().Append(scriptsItem);
        fileItem.Items().Append(MenuFlyoutSeparator());

        Input::KeyboardAccelerator captureImageKeyboardAccelerator;
        captureImageKeyboardAccelerator.Key(VirtualKey::F10);
        AppendItem(fileItem, LocalizationHelper::Localize(L"Capture Image", L"Take a screenshot in Celestia"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->CaptureImage();
            }, captureImageKeyboardAccelerator);

        fileItem.Items().Append(MenuFlyoutSeparator());

        Input::KeyboardAccelerator copyKeyboardAccelerator;
        copyKeyboardAccelerator.Key(VirtualKey::C);
        copyKeyboardAccelerator.Modifiers(VirtualKeyModifiers::Control);
        AppendItem(fileItem, LocalizationHelper::Localize(L"Copy URL", L"Copy current URL to pasteboard"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->CopyURL();
            }, copyKeyboardAccelerator);
        Input::KeyboardAccelerator pasteKeyboardAccelerator;
        pasteKeyboardAccelerator.Key(VirtualKey::V);
        pasteKeyboardAccelerator.Modifiers(VirtualKeyModifiers::Control);
        AppendItem(fileItem, LocalizationHelper::Localize(L"Paste URL", L"Paste URL from pasteboard"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->PasteURL();
            }, pasteKeyboardAccelerator);
        fileItem.Items().Append(MenuFlyoutSeparator());

        AppendItem(fileItem, LocalizationHelper::Localize(L"Settings", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowSettings();
            });
        fileItem.Items().Append(MenuFlyoutSeparator());

        if (!isXbox)
        {
            AppendItem(fileItem, LocalizationHelper::Localize(L"Open Custom Folder", L"Open folder for customization"), [](IInspectable const&, RoutedEventArgs const&)
                {
                    Launcher::LaunchFolderAsync(ApplicationData::Current().LocalFolder());
                });
            fileItem.Items().Append(MenuFlyoutSeparator());
        }

        AppendItem(fileItem, LocalizationHelper::Localize(L"Exit", L"Exit Celestia"), [](IInspectable const&, RoutedEventArgs const&)
            {
                Application::Current().Exit();
            });
        MenuBarItem navigationItem;
        navigationItem.Title(LocalizationHelper::Localize(L"Navigation", L"Navigation menu"));

        AppendItem(navigationItem, LocalizationHelper::Localize(L"Get Info", L"Action for getting info about current selected object"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->renderer.EnqueueTask([strong_this]()
                    {
                        auto selection = strong_this->appCore.Simulation().Selection();
                        if (selection.IsEmpty()) return;
                        strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [strong_this, selection]()
                            {
                                strong_this->ShowInfo(selection);
                            });
                    });
            });
        navigationItem.Items().Append(MenuFlyoutSeparator());

        AppendCharEnterItem(navigationItem, LocalizationHelper::Localize(L"Select Sol", L""), 104, appCore, renderer, VirtualKey::H);
        AppendItem(navigationItem, LocalizationHelper::Localize(L"Search\u2026", L"Menu item to start searching"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowSearch();
            });
        AppendItem(navigationItem, LocalizationHelper::Localize(L"Go to Object", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowGotoObject();
            });
        navigationItem.Items().Append(MenuFlyoutSeparator());


        std::vector<std::pair<hstring, int16_t>> actions =
        {
            {LocalizationHelper::Localize(L"Center Selection", L"Center selected object"), (int16_t)99},
            {LocalizationHelper::Localize(L"Go to Selection", L"Go to selected object"), (int16_t)103},
            {LocalizationHelper::Localize(L"Follow Selection", L""), (int16_t)102},
            {LocalizationHelper::Localize(L"Sync Orbit Selection", L""), (int16_t)121},
            {LocalizationHelper::Localize(L"Track Selection", L"Track selected object"), (int16_t)116}
        };

        for (const auto& [name, code] : actions)
        {
            AppendCharEnterItem(navigationItem, name, code, appCore, renderer, (VirtualKey)(code - 32));
        }
        navigationItem.Items().Append(MenuFlyoutSeparator());

        AppendItem(navigationItem, LocalizationHelper::Localize(L"Flight Mode", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowObserverMode();
            });

        MenuBarItem toolsItem;
        toolsItem.Title(LocalizationHelper::Localize(L"Tools", L"Tools menu title"));
        AppendItem(toolsItem, LocalizationHelper::Localize(L"Tour Guide", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowTourGuide();
            });
        AppendItem(toolsItem, LocalizationHelper::Localize(L"Star Browser", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowBrowser();
            });
        AppendItem(toolsItem, LocalizationHelper::Localize(L"Eclipse Finder", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowEclipseFinder();
            });
        toolsItem.Items().Append(MenuFlyoutSeparator());
        AppendItem(toolsItem, LocalizationHelper::Localize(L"Get Add-ons", L"Open webpage for downloading add-ons"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowOnlineAddons();
            });
        AppendItem(toolsItem, LocalizationHelper::Localize(L"Installed Add-ons", L"Open a page for managing installed add-ons"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowAddonManagement();
            });

        if (!isXbox)
        {
            toolsItem.Items().Append(MenuFlyoutSeparator());
            AppendItem(toolsItem, LocalizationHelper::Localize(L"Open Add-on Folder", L"Open the folder for add-ons"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
                {
                    auto strong_this{ weak_this.get() };
                    if (strong_this == nullptr) return;
                    auto folder = strong_this->extraAddonFolder;
                    if (folder != nullptr)
                        Launcher::LaunchFolderAsync(folder);
                });
            AppendItem(toolsItem, LocalizationHelper::Localize(L"Open Script Folder", L"Open the folder for scripts"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
                {
                    auto strong_this{ weak_this.get() };
                    if (strong_this == nullptr) return;
                    auto folder = strong_this->extraScriptFolder;
                    if (folder != nullptr)
                        Launcher::LaunchFolderAsync(folder);
                });
        }

        MenuBarItem timeItem;
        timeItem.Title(LocalizationHelper::Localize(L"Time", L""));
        AppendCharEnterItem(timeItem, LocalizationHelper::Localize(L"10x Faster", L"10x time speed"), 108, appCore, renderer, VirtualKey::L);
        AppendCharEnterItem(timeItem, LocalizationHelper::Localize(L"10x Slower", L"0.1x time speed"), 107, appCore, renderer, VirtualKey::K);
        AppendCharEnterItem(timeItem, LocalizationHelper::Localize(L"Freeze", L"Freeze time"), 32, appCore, renderer, VirtualKey::Space);
        AppendCharEnterItem(timeItem, LocalizationHelper::Localize(L"Real Time", L"Reset time speed to 1x"), 33, appCore, renderer);
        AppendCharEnterItem(timeItem, LocalizationHelper::Localize(L"Reverse Time", L""), 106, appCore, renderer, VirtualKey::J);

        timeItem.Items().Append(MenuFlyoutSeparator());

        AppendItem(timeItem, LocalizationHelper::Localize(L"Set Time\u2026", L"Select simulation time"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowTimeSetting();
            });

        MenuBarItem viewItem;
        viewItem.Title(LocalizationHelper::Localize(L"Views", L"Menu for split views"));
        AppendCharEnterItem(viewItem, LocalizationHelper::Localize(L"Split Horizontally", L"Split view"), 18, appCore, renderer, VirtualKey::R, VirtualKeyModifiers::Control);
        AppendCharEnterItem(viewItem, LocalizationHelper::Localize(L"Split Vertically", L"Split view"), 21, appCore, renderer, VirtualKey::U, VirtualKeyModifiers::Control);
        AppendCharEnterItem(viewItem, LocalizationHelper::Localize(L"Delete Active View", L"Delete current view (in split view mode)"), 127, appCore, renderer, VirtualKey::Delete);
        AppendCharEnterItem(viewItem, LocalizationHelper::Localize(L"Delete Other Views", L"Delete views other than current view (in split view mode)"), 4, appCore, renderer, VirtualKey::D, VirtualKeyModifiers::Control);
        viewItem.Items().Append(MenuFlyoutSeparator());
        Input::KeyboardAccelerator fullScreenKeyboardAccelerator;
        fullScreenKeyboardAccelerator.Key(VirtualKey::F11);
        AppendItem(viewItem, LocalizationHelper::Localize(L"Toggle Fullscreen", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ToggleFullScreen();
            }, fullScreenKeyboardAccelerator);

        MenuBarItem bookmarkItem;
        bookmarkItem.Title(LocalizationHelper::Localize(L"Bookmarks", L"URL bookmarks"));
        AppendItem(bookmarkItem, LocalizationHelper::Localize(L"Add Bookmark", L"Add a new bookmark"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowNewBookmark();
            });
        AppendItem(bookmarkItem, LocalizationHelper::Localize(L"Organize Bookmarks\u2026", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowBookmarkOrganizer();
            });

        MenuBarItem helpItem;
        helpItem.Title(LocalizationHelper::Localize(L"Help", L""));
        AppendItem(helpItem, LocalizationHelper::Localize(L"Run Demo", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->renderer.EnqueueTask([strong_this]()
                    {
                        strong_this->appCore.RunDemo();
                    });
            });
        helpItem.Items().Append(MenuFlyoutSeparator());
        AppendItem(helpItem, LocalizationHelper::Localize(L"OpenGL Info", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->renderer.EnqueueTask([strong_this]()
                    {
                        auto renderInfo = strong_this->appCore.RenderInfo();
                        strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [strong_this, renderInfo]()
                            {
                                ContentDialogHelper::ShowText(strong_this->Content(), LocalizationHelper::Localize(L"OpenGL Info", L""), renderInfo);
                            });
                    });
            });

        if (!isXbox)
        {
            helpItem.Items().Append(MenuFlyoutSeparator());
            AppendItem(helpItem, LocalizationHelper::Localize(L"Report a Bug", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
                {
                    auto strong_this{ weak_this.get() };
                    if (strong_this == nullptr) return;
                    strong_this->ReportBug();
                });
            AppendItem(helpItem, LocalizationHelper::Localize(L"Suggest a Feature", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
                {
                    auto strong_this{ weak_this.get() };
                    if (strong_this == nullptr) return;
                    strong_this->SuggestFeature();
                });
        }

        helpItem.Items().Append(MenuFlyoutSeparator());
        AppendItem(helpItem, LocalizationHelper::Localize(L"Celestia Help", L""), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowHelp();
            });
        AppendItem(helpItem, LocalizationHelper::Localize(L"About Celestia", L"System menu item"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                AboutDialog dialog{ PathHelper::Combine(strong_this->defaultResourcePath, L"AUTHORS"), PathHelper::Combine(strong_this->defaultResourcePath, L"TRANSLATORS") };
                ContentDialogHelper::ShowContentDialogAsync(strong_this->Content(), dialog);
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
            auto object = dialog.Object();
            if (object.IsEmpty())
            {
                ShowObjectNotFound();
                co_return;
            }

            auto latitude = dialog.Latitude();
            auto longitude = dialog.Longitude();
            auto distance = dialog.Distance();
            auto unit = dialog.UnitType();
            renderer.EnqueueTask([this, object, latitude, longitude, distance, unit]
                {
                    CelestiaGotoLocation location{ object, latitude, longitude, distance, unit };
                    appCore.Simulation().GoToLocation(location);
                });
        }
    }

    IAsyncAction MainWindow::ShowObjectNotFound()
    {
        co_await ContentDialogHelper::ShowAlert(Content(), LocalizationHelper::Localize(L"Object not found", L""));
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
        if (auto file = co_await picker.PickSingleFileAsync(); file != nullptr)
            OpenFileIfReady(file);
    }

    void MainWindow::OpenFileIfReady(StorageFile const scriptFile)
    {
        auto dispatcherQueue{ DispatcherQueue() };
        if (dispatcherQueue == nullptr) return;
        dispatcherQueue.TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, scriptFile]()
            {
                scriptFileToOpen = scriptFile;
                urlToOpen = nullptr;
                if (readyForInput)
                    OpenFileOrURL();
            });
    }

    void MainWindow::OpenURLIfReady(Uri const url)
    {
        auto dispatcherQueue{ DispatcherQueue() };
        if (dispatcherQueue == nullptr) return;
        dispatcherQueue.TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, url]()
            {
                urlToOpen = url;
                scriptFileToOpen = nullptr;
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
            if (co_await ContentDialogHelper::ShowOption(Content(), LocalizationHelper::Localize(L"Run script?", L"Request user consent to run a script")))
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
                if (co_await ContentDialogHelper::ShowOption(Content(), LocalizationHelper::Localize(L"Open URL?", L"Request user consent to open a URL")))
                {
                    renderer.EnqueueTask([this, url]()
                        {
                            appCore.GoToURL(url.AbsoluteUri());
                        });
                }
                co_return;
            }

            if (resourceManager != nullptr && url.SchemeName() == L"celaddon" && url.Host() == L"item" && !url.Query().empty())
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
                                window.Title(item.Name());
                                ResourceItemUserControl userControl{ appCore, renderer, item, resourceManager, [weak_window{ make_weak(window) }]()
                                    {
                                        return weak_window.get();
                                    }
                                };
                                window.Content(userControl);
                                WindowHelper::SetWindowIcon(window);
                                WindowHelper::SetWindowTheme(window);
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

            if (url.SchemeName() == L"celguide" && url.Host() == L"guide" && !url.Query().empty())
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
                        SafeWebUserControl userControl{ CelestiaWinUI::CommonWebParameter(GetURIForGuide(guide), single_threaded_vector(std::vector<hstring>({ L"guide" })), appCore, renderer, L"", nullptr, [weak_window{ make_weak(window)}]()
                            {
                                return weak_window.get();
                            }, true) };
                        window.Content(userControl);
                        WindowHelper::SetWindowIcon(window);
                        WindowHelper::SetWindowTheme(window);
                        WindowHelper::SetWindowFlowDirection(window);
                        WindowHelper::ResizeWindow(window, 400, 600);
                        WindowHelper::TrackWindow(window, guide);
                        window.Activate();
                    }
                }
                co_return;
            }
        }


        if (!appSettings.OnboardMessageDisplayed())
        {
            appSettings.OnboardMessageDisplayed(true);
            appSettings.Save(ApplicationData::Current().LocalSettings());
            ShowHelp();
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
                if (appSettings.LastNewsID() != item.ID())
                {
                    auto trackedWindow = WindowHelper::GetTrackedWindow(item.ID());
                    if (trackedWindow != nullptr)
                    {
                        trackedWindow.Activate();
                        co_return;
                    }
                    Window window;
                    window.SystemBackdrop(Media::MicaBackdrop());
                    window.Title(item.Title());
                    SafeWebUserControl userControl{ CelestiaWinUI::CommonWebParameter(GetURIForGuide(item.ID()), single_threaded_vector(std::vector<hstring>({ L"guide" })), appCore, renderer, L"", [weak_this{ get_weak() }](hstring const& id)
                        {
                            auto strong_this = weak_this.get();
                            if (strong_this)
                            {
                                strong_this->appSettings.LastNewsID(id);
                                strong_this->appSettings.Save(ApplicationData::Current().LocalSettings());
                            }
                        }, [weak_window{ make_weak(window)}]()
                        {
                            return weak_window.get();
                        }) };
                    window.Content(userControl);
                    WindowHelper::SetWindowIcon(window);
                    WindowHelper::SetWindowTheme(window);
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

    void MainWindow::CopyURL()
    {
        using namespace Windows::ApplicationModel::DataTransfer;
        renderer.EnqueueTask([this]()
            {
                auto url = appCore.CurrentURL();
                DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [url]()
                    {
                        DataPackage dataPackage;
                        dataPackage.RequestedOperation(DataPackageOperation::Copy);
                        dataPackage.SetText(url);
                        Clipboard::SetContent(dataPackage);
                    });
            });
    }

    IAsyncAction MainWindow::PasteURL()
    {
        using namespace Windows::ApplicationModel::DataTransfer;
        auto dataPackageView = Clipboard::GetContent();
        if (dataPackageView.Contains(StandardDataFormats::Text()))
        {
            auto text = co_await dataPackageView.GetTextAsync();
            OpenURLIfReady(Uri(text));
        }
    }

    void MainWindow::ShowSearch()
    {
        const hstring id = L"Search";
        auto trackedWindow = WindowHelper::GetTrackedWindow(id);
        if (trackedWindow != nullptr)
        {
            trackedWindow.Activate();
            return;
        }
        SearchUserControl userControl{ appCore, renderer };
        Window window;
        window.SystemBackdrop(Media::MicaBackdrop());
        window.Title(LocalizationHelper::Localize(L"Search", L""));
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowTheme(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 500, 500);
        WindowHelper::TrackWindow(window, id);
        window.Activate();
    }

    void MainWindow::CaptureImage()
    {
        auto tempFolder{ ApplicationData::Current().TemporaryFolder() };
        auto path = PathHelper::Combine(tempFolder.Path(), to_hstring(GuidHelper::CreateNewGuid()) + L".png");
        renderer.EnqueueTask([this, path]()
            {
                if (appCore.SaveScreenshot(path))
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
                            ContentDialogHelper::ShowAlert(Content(), LocalizationHelper::Localize(L"Failed in capturing screenshot.", L""));
                        });
                }
            });
    }

    IAsyncAction MainWindow::SaveScreenshot(hstring const path)
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
            savePicker.FileTypeChoices().Insert(LocalizationHelper::Localize(L"Image", L"Sharing option, image"), fileExtensions);
            // Default file name if the user does not type one in or select a file to replace
            savePicker.SuggestedFileName(LocalizationHelper::Localize(L"Celestia Screenshot", L""));
            auto initializeWithWindow{ savePicker.as<::IInitializeWithWindow>() };
            initializeWithWindow->Initialize(hWnd);

            StorageFile file{ co_await savePicker.PickSaveFileAsync() };
            if (file == nullptr) co_return;

            co_await originalFile.CopyAndReplaceAsync(file);
        }
        catch (hresult_error const&)
        {
            ContentDialogHelper::ShowAlert(Content(), LocalizationHelper::Localize(L"Failed in capturing screenshot.", L""));
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
        window.Title(LocalizationHelper::Localize(L"Tour Guide", L""));
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowTheme(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 700, 400);
        WindowHelper::TrackWindow(window, id);
        window.Activate();
    }

    void MainWindow::ShowInfo(CelestiaSelection const& selection)
    {
        // No need for tracking...
        InfoUserControl userControl{ appCore, renderer, selection };
        Window window;
        window.SystemBackdrop(Media::MicaBackdrop());
        window.Content(userControl);
        window.Title(appCore.Simulation().Universe().NameForSelection(selection));
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowTheme(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 700, 400);
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
        window.Title(LocalizationHelper::Localize(L"Eclipse Finder", L""));
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowTheme(window);
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
        auto referenceObject = dialog.ReferenceObject();
        auto targetObject = dialog.TargetObject();
        renderer.EnqueueTask([this, coordinateSystem, referenceObject, targetObject]()
            {
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
        window.Title(LocalizationHelper::Localize(L"Star Browser", L""));
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowTheme(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 700, 400);
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
        window.Title(LocalizationHelper::Localize(L"Bookmark Organizer", L""));
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowTheme(window);
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
        window.Title(LocalizationHelper::Localize(L"Add Bookmark", L"Add a new bookmark"));
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowTheme(window);
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
        SettingsUserControl userControl{ appCore, renderer, appSettings, ApplicationData::Current().LocalSettings(), languages };
        Window window;
        window.SystemBackdrop(Media::MicaBackdrop());
        window.Title(LocalizationHelper::Localize(L"Settings", L""));
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowTheme(window);
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
        window.Title(LocalizationHelper::Localize(L"Manage Add-ons", L"window title for add-on list"));
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowTheme(window);
        WindowHelper::SetWindowFlowDirection(window);
        WindowHelper::ResizeWindow(window, 400, 600);
        WindowHelper::TrackWindow(window, id);
        window.Activate();
    }

    fire_and_forget MainWindow::ShowOnlineAddons()
    {
        std::map<hstring, hstring> content;
        content.emplace(L"lang", LocalizationHelper::Locale());
        auto encoder = Windows::Web::Http::HttpFormUrlEncodedContent(content);
        hstring url = hstring(L"https://celestia.mobi/resources/categories") + L"?" + encoder.ToString();
        co_await Launcher::LaunchUriAsync(Uri(url));
    }

    void MainWindow::ToggleFullScreen()
    {
        auto appWindow = AppWindow();
        if (appWindow == nullptr) return;

        using namespace Microsoft::UI::Windowing;
        bool isFullScreen = appWindow.Presenter().Kind() == AppWindowPresenterKind::FullScreen;
        appWindow.SetPresenter(isFullScreen ? AppWindowPresenterKind::Default : AppWindowPresenterKind::FullScreen);
        isFullScreen = !isFullScreen;
        MenuBar().Visibility(isFullScreen ? Visibility::Collapsed : Visibility::Visible);
    }

    void MainWindow::ShowHelp()
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
        SafeWebUserControl userControl{ CelestiaWinUI::CommonWebParameter(GetURIForPath(L"/help/welcome"), single_threaded_vector<hstring>(), appCore, renderer, L"", nullptr, [weak_window{make_weak(window)}]()
            {
                return weak_window.get();
            }, true)
        };
        window.Content(userControl);
        WindowHelper::SetWindowIcon(window);
        WindowHelper::SetWindowTheme(window);
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
                    bool saveScreenshotSuccess = appCore.SaveScreenshot(screenshotFilePath);
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

            if (!installedAddonList.empty())
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
            if (screenshotFile != nullptr)
                attachments.Append(screenshotFile);
            attachments.Append(renderInfoFile);
            attachments.Append(urlInfoFile);
            attachments.Append(systemInfoFile);
            if (!installedAddonList.empty())
                attachments.Append(addonInfoFile);
            if (!co_await SendEmail(LocalizationHelper::Localize(L"Bug report for Celestia", L""), LocalizationHelper::Localize(L"Please describe the issue and repro steps, if known.", L"Default email body for bug report"), feedbackEmailAddress, attachments))
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
        if (!co_await SendEmail(LocalizationHelper::Localize(L"Feature suggestion for Celestia", L"Default email title for feature suggestion"), LocalizationHelper::Localize(L"Please describe the feature you want to see in Celestia.", L"Default email body for feature suggestion"), feedbackEmailAddress, single_threaded_vector<StorageFile>()))
        {
            ReportBugOrSuggestFeatureFallback();
        }
    }

    fire_and_forget MainWindow::ReportBugOrSuggestFeatureFallback()
    {
        co_await Launcher::LaunchUriAsync(Uri(feedbackLinkAddress));
    }

    fire_and_forget MainWindow::ShowSystemAccessRequest(HANDLE semaphore, SystemAccessRequestArgs const args)
    {
        bool result = co_await ContentDialogHelper::ShowOption(
            Content(),
            LocalizationHelper::Localize(L"Script System Access", L"Alert title for scripts requesting system access"),
            LocalizationHelper::Localize(L"This script requests permission to read/write files and execute external programs. Allowing this can be dangerous.\nDo you trust the script and want to allow this?", L"Alert message for scripts requesting system access")
        );
        args.Result(result ? SystemAccessRequestResult::Granted : SystemAccessRequestResult::Denied);
        ReleaseSemaphore(semaphore, 1, nullptr);
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

                AppendItem(menu, LocalizationHelper::Localize(L"Get Info", L"Action for getting info about current selected object"), [weak_this{ get_weak() }, selection](IInspectable const&, RoutedEventArgs const&)
                    {
                        auto strong_this{ weak_this.get() };
                        if (strong_this == nullptr) return;
                        strong_this->ShowInfo(selection);
                    });

                menu.Items().Append(MenuFlyoutSeparator());

                std::vector<std::pair<hstring, int16_t>> actions = { {LocalizationHelper::Localize(L"Go", L"Go to an object"), (int16_t)103}, {LocalizationHelper::Localize(L"Follow", L""), (int16_t)102}, {LocalizationHelper::Localize(L"Sync Orbit", L""), (int16_t)121} };
                for (const auto& [name, code] : actions)
                {
                    auto copiedCode = code;
                    AppendItem(menu, name, [weak_this{ get_weak() }, selection, copiedCode](IInspectable const&, RoutedEventArgs const&)
                        {
                            auto strong_this{ weak_this.get() };
                            if (strong_this == nullptr) return;
                            strong_this->renderer.EnqueueTask([strong_this, selection, copiedCode]()
                                {
                                    strong_this->appCore.Simulation().Selection(selection);
                                    strong_this->appCore.CharEnter(copiedCode);
                                });
                        });
                }
                CelestiaBody body = selection.Object().try_as<CelestiaBody>();
                if (body != nullptr)
                {
                    PopulateBodyMenu(body, menu, appCore, renderer);
                }

                std::vector<MenuFlyoutItemBase> browserMenuItems;
                CelestiaBrowserItem browserItem{ appCore.Simulation().Universe().NameForSelection(selection), selection.Object(), [weak_this{ get_weak() }](CelestiaBrowserItem const& item)
                    {
                        auto strong_this{ weak_this.get() };
                        if (strong_this == nullptr) return com_array<CelestiaBrowserItem>();
                        return CelestiaExtension::GetChildren(item, strong_this->appCore);
                    }, false };
                if (auto children = browserItem.Children(); children != nullptr && children.Size() > 0)
                {
                    for (const auto& child : children)
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

                MenuFlyoutSubItem markMenu;
                BrowserMarkAction markAction;
                markMenu.Text(markAction.Name());

                for (const auto menuItem : markAction.MenuItems())
                {
                    AppendSubItem(markMenu, menuItem.Title(), [weak_this{get_weak()}, selection, menuItem](IInspectable const&, RoutedEventArgs const&)
                        {
                            auto strong_this{ weak_this.get() };
                            if (strong_this == nullptr) return;
                            strong_this->renderer.EnqueueTask([strong_this, selection, menuItem]()
                                {
                                    if (!menuItem.Mark())
                                    {
                                        strong_this->appCore.Simulation().Universe().UnmarkSelection(selection);
                                    }
                                    else
                                    {
                                        strong_this->appCore.Simulation().Universe().MarkSelection(selection, menuItem.Marker());
                                        strong_this->appCore.ShowMarkers(true);
                                    }
                                });
                        });
                }
                menu.Items().Append(markMenu);

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

    void MainWindow::AppCore_SystemAccessRequest(IInspectable const&, SystemAccessRequestArgs const& args)
    {
        auto sem = CreateSemaphore(nullptr, 0, 1, nullptr);
        DispatcherQueue().TryEnqueue([weak_this{ get_weak() }, sem, args]()
            {
                auto strong_this{ weak_this.get() };
                if (!strong_this)
                {
                    ReleaseSemaphore(sem, 1, nullptr);
                    return;
                }
                strong_this->ShowSystemAccessRequest(sem, args);
            });
        WaitForSingleObject(sem, INFINITE);
        CloseHandle(sem);
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
                    GLView().as<IUIElementProtected>().ProtectedCursor(Microsoft::UI::Input::InputSystemCursor::Create(Microsoft::UI::Input::InputSystemCursorShape::Arrow));
                    GLView().as<IUIElementProtected>().ProtectedCursor().Close();
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
                    GLView().as<IUIElementProtected>().ProtectedCursor(Microsoft::UI::Input::InputSystemCursor::Create(Microsoft::UI::Input::InputSystemCursorShape::Arrow));
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
            if (element != nullptr && !isClosed && element.XamlRoot() == Content().XamlRoot())
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
                appCore.KeyDown(static_cast<int32_t>(key), modifiers);
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

        auto key = static_cast<int32_t>(args.Key());

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
            altSur.Text(LocalizationHelper::Localize(L"Alternate Surfaces", L"Alternative textures to display"));
            AppendSubItem(altSur, LocalizationHelper::Localize(L"Default", L""), [appCore, renderer](IInspectable const&, RoutedEventArgs const&)
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
        refMarkMenu.Text(LocalizationHelper::Localize(L"Reference Vectors", L"Reference vectors for an object"));


        enum class RefMarkMenuActionType
        {
            ShowBodyAxes, ShowFrameAxes, ShowSunDirection, ShowVelocityVector, ShowPlanetographicGrid, ShowTerminator
        };

        std::vector<std::pair<hstring, RefMarkMenuActionType>> refMarkMenuActions =
        {
            {LocalizationHelper::Localize(L"Show Body Axes", L"Reference vector"), RefMarkMenuActionType::ShowBodyAxes},
            {LocalizationHelper::Localize(L"Show Frame Axes", L"Reference vector"), RefMarkMenuActionType::ShowFrameAxes},
            {LocalizationHelper::Localize(L"Show Sun Direction", L"Reference vector"), RefMarkMenuActionType::ShowSunDirection},
            {LocalizationHelper::Localize(L"Show Velocity Vector", L"Reference vector"), RefMarkMenuActionType::ShowVelocityVector},
            {LocalizationHelper::Localize(L"Show Planetographic Grid", L"Reference vector"), RefMarkMenuActionType::ShowPlanetographicGrid},
            {LocalizationHelper::Localize(L"Show Terminator", L"Reference vector"), RefMarkMenuActionType::ShowTerminator}
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
            AppendToggleSubItem(refMarkMenu, name, isEnabled, [appCore, renderer, copiedType, isEnabled](IInspectable const&, RoutedEventArgs const&)
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

    MenuFlyoutItemBase CreateMenuItem(CelestiaBrowserItem const item, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer)
    {
        MenuFlyoutSubItem menu;
        menu.Text(item.Name());

        menu.Loaded([appCore, renderer, item](IInspectable const& sender, RoutedEventArgs const&)
            {
                auto senderMenu = sender.as<MenuFlyoutSubItem>();
                if (auto subItems = senderMenu.Items(); subItems != nullptr && subItems.Size() > 0) return;

                std::vector<MenuFlyoutItemBase> children;
                auto obj = item.Object();
                if (obj != nullptr)
                {
                    MenuFlyoutItem selectItem;
                    selectItem.Text(LocalizationHelper::Localize(L"Select", L"Select an object"));
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
                if (auto items = item.Children(); items != nullptr && items.Size() > 0)
                {
                    children.push_back(MenuFlyoutSeparator());

                    for (const auto child : items)
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