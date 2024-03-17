//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

#include <fmt/format.h>
#include <fmt/printf.h>
#include <fmt/xchar.h>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Globalization;
using namespace Windows::Storage;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::CelestiaUWP2::implementation
{
    MainPage::MainPage()
    {
        CelestiaAppCore::SetUpLocale();
        appCore = CelestiaAppCore();

        isXbox = Windows::System::Profile::AnalyticsInfo::VersionInfo().DeviceFamily() == L"Windows.Xbox";

        appSettings = AppSettings(ApplicationData::Current().LocalSettings());

        defaultParentPath = Windows::ApplicationModel::Package::Current().InstalledLocation().Path();
        defaultResourcePath = PathHelper::Combine(defaultParentPath, L"CelestiaResources");
        defaultConfigFilePath = PathHelper::Combine(defaultResourcePath, L"celestia.cfg");
    }

    void MainPage::InitializeComponent()
    {
        MainPageT::InitializeComponent();
        auto coreTitleBar = Windows::ApplicationModel::Core::CoreApplication::GetCurrentView().TitleBar();
        coreTitleBar.ExtendViewIntoTitleBar(false);
    }

    void MainPage::WillActivate(Windows::ApplicationModel::Activation::IActivatedEventArgs const& args)
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

    void MainPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        WillActivate(e.Parameter().try_as<Windows::ApplicationModel::Activation::IActivatedEventArgs>());
    }

    void MainPage::MainPage_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        if (!readyForInput || isXbox) return;

        auto isFullScreen = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().IsFullScreenMode();
        MenuBar().Visibility(isFullScreen ? Visibility::Collapsed : Visibility::Visible);
    }

    void MainPage::SystemNavigationManager_BackRequested(IInspectable const&, Windows::UI::Core::BackRequestedEventArgs const& args)
    {
        if (OverlayContainer().Content() != nullptr)
        {
            args.Handled(true);
            ClosePanel();
            return;
        }
        if (MenuBar().Visibility() == Visibility::Visible)
        {
            args.Handled(true);
            MenuBar().Visibility(Visibility::Collapsed);
            FocusHelperControl().Focus(FocusState::Programmatic);
            return;
        }
        args.Handled(true);
    }

    fire_and_forget MainPage::MainPage_Loaded(IInspectable const& sender, RoutedEventArgs const& e)
    {
        if (appSettings.UseFullDPI())
            scale = static_cast<int>(Windows::Graphics::Display::DisplayInformation::GetForCurrentView().ResolutionScale()) / 100.0f;

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

            Windows::ApplicationModel::Resources::Core::ResourceContext::GetForCurrentView().Reset();
            Windows::ApplicationModel::Resources::Core::ResourceContext::GetForViewIndependentUse().Reset();
        }

        auto resourceLoader = Windows::ApplicationModel::Resources::ResourceLoader::GetForViewIndependentUse();
        auto flowDirection = resourceLoader.GetString(L"ApplicationFlowDirection");
        FlowDirection(flowDirection == L"RightToLeft" ? FlowDirection::RightToLeft : FlowDirection::LeftToRight);
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
        renderer.Start();
    }

    bool MainPage::StartEngine(hstring const& resourcePath, hstring const& configPath, hstring const& locale, CelestiaLayoutDirection layoutDirection, JsonObject const& defaultSettings)
    {
        CelestiaAppCore::InitGL();

        std::vector<hstring> extraPaths;
        if (!extraAddonFolderPath.empty())
            extraPaths.push_back(extraAddonFolderPath);

        SetCurrentDirectoryW(resourcePath.c_str());
        hstring localeDirectory = PathHelper::Combine(resourcePath, L"locale");
        CelestiaAppCore::SetLocaleDirectory(localeDirectory, locale);

        bool loadSuccess = appCore.StartSimulation(configPath, extraPaths, [this](hstring const& status) {
            Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, status]()
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
                Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this]()
                    {
                        ContentDialogHelper::ShowAlert(*this, LocalizationHelper::Localize(L"Error loading data, fallback to original configuration.", L""));
                    });
                SetCurrentDirectoryW(defaultResourcePath.c_str());
                CelestiaAppCore::SetLocaleDirectory(PathHelper::Combine(defaultResourcePath, L"locale"), locale);
                if (!appCore.StartSimulation(defaultConfigFilePath, extraPaths, [this](hstring const& status) {
                    Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, status]()
                        {
                            auto textToDisplay = hstring(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Loading: %s", L"Celestia initialization, loading file")), std::wstring(status)));
                            LoadingText().Text(textToDisplay);
                        });
                    }))
                {
                    Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this]()
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
            Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this]()
                {
                    ShowLoadingFailure();
                });
            return false;
        }

        LocalizationHelper::Locale(CelestiaAppCore::Language());
        appCore.LayoutDirection(layoutDirection);
        UpdateScale();

        Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, resourcePath]()
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

        Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this]()
            {
                OpenFileOrURL();
            });
        return true;
    }

    IAsyncAction MainPage::CreateExtraFolders()
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

    IAsyncOperation<JsonObject> MainPage::ReadDefaultSettings()
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

    void MainPage::ApplySettings(JsonObject const& defaultSettings)
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

    void MainPage::ShowLoadingFailure()
    {
        LoadingText().Text(LocalizationHelper::Localize(L"Loading Celestia failed\u2026", L"Celestia loading failed"));
    }

    void MainPage::UpdateScale()
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

    IAsyncOperation<hstring> MainPage::GetLocale(hstring const& localePath)
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
    void AppendItem(Microsoft::UI::Xaml::Controls::MenuBarItem const& parent, hstring const& text, RoutedEventHandler const& click, Input::KeyboardAccelerator const& accelerator = nullptr);
    void AppendSubItem(MenuFlyoutSubItem const& parent, hstring const& text, RoutedEventHandler click);
    void AppendToggleSubItem(MenuFlyoutSubItem const& parent, hstring const& text, bool isChecked, RoutedEventHandler const& click);
    void AppendCharEnterItem(Microsoft::UI::Xaml::Controls::MenuBarItem const& item, hstring const& title, short input, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, std::optional<VirtualKey> key = std::nullopt, std::optional<VirtualKeyModifiers> modifiers = std::nullopt);
    void PopulateBodyMenu(CelestiaBody const& body, MenuFlyout const& menu, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer);
    MenuFlyoutItemBase CreateMenuItem(CelestiaBrowserItem const item, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer);

    void MainPage::SetUpGLViewInteractions()
    {
        appCore.ShowContextMenu({ get_weak(), &MainPage::AppCore_ShowContextMenu });
        appCore.FatalError({ get_weak(), &MainPage::AppCore_FatalError });
        GLView().PointerPressed({ get_weak(), &MainPage::GLView_PointerPressed });
        GLView().PointerMoved({ get_weak(), &MainPage::GLView_PointerMoved });
        GLView().PointerReleased({ get_weak(), &MainPage::GLView_PointerReleased });
        GLView().PointerWheelChanged({ get_weak(), &MainPage::GLView_PointerWheelChanged });

        FocusHelperControl().Focus(FocusState::Programmatic);
        isGLViewFocused = true;
        Input::FocusManager::GotFocus({ get_weak(), &MainPage::FocusManager_GotFocus });
        FocusHelperControl().CharacterReceived({ get_weak(), &MainPage::FocusHelperControl_CharacterReceived });
        FocusHelperControl().KeyDown({ get_weak(), &MainPage::FocusHelperControl_KeyDown });
        FocusHelperControl().KeyUp({ get_weak(), &MainPage::FocusHelperControl_KeyUp });
        MenuBar().KeyUp({ get_weak() , &MainPage::MenuBar_KeyUp });

        gamepadManager = GamepadManager(appSettings);
        renderer.SetPreRenderTask([weak_this{ get_weak() }]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                if (!strong_this->isGLViewFocused || strong_this->hasOverlayOpen) return;
                strong_this->gamepadManager.PollGamepad(strong_this->appCore);
            });

        Windows::UI::Core::SystemNavigationManager::GetForCurrentView().BackRequested({ get_weak(), &MainPage::SystemNavigationManager_BackRequested });
    }

    void MainPage::PopulateMenuBar(hstring const& resourcePath)
    {
        auto isFullScreen = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().IsFullScreenMode();
        MenuBar().Visibility((isFullScreen || isXbox) ? Visibility::Collapsed : Visibility::Visible);

        Microsoft::UI::Xaml::Controls::MenuBarItem fileItem;
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
        Microsoft::UI::Xaml::Controls::MenuBarItem navigationItem;
        navigationItem.Title(LocalizationHelper::Localize(L"Navigation", L"Navigation menu"));
        AppendCharEnterItem(navigationItem, LocalizationHelper::Localize(L"Select Sol", L""), 104, appCore, renderer, VirtualKey::H);
        AppendItem(navigationItem, LocalizationHelper::Localize(L"Select Object", L"Select an object"), [weak_this{ get_weak() }](IInspectable const&, RoutedEventArgs const&)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->ShowSelectObject();
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

        Microsoft::UI::Xaml::Controls::MenuBarItem toolsItem;
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

        Microsoft::UI::Xaml::Controls::MenuBarItem timeItem;
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

        Microsoft::UI::Xaml::Controls::MenuBarItem viewItem;
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

        Microsoft::UI::Xaml::Controls::MenuBarItem bookmarkItem;
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

        Microsoft::UI::Xaml::Controls::MenuBarItem helpItem;
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
                        strong_this->Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [strong_this, renderInfo]()
                            {
                                ContentDialogHelper::ShowText(*strong_this, LocalizationHelper::Localize(L"OpenGL Info", L""), renderInfo);
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
                ContentDialogHelper::ShowContentDialogAsync(*strong_this, dialog);
            });

        MenuBar().Items().Append(fileItem);
        MenuBar().Items().Append(navigationItem);
        MenuBar().Items().Append(timeItem);
        MenuBar().Items().Append(toolsItem);
        MenuBar().Items().Append(viewItem);
        MenuBar().Items().Append(bookmarkItem);
        MenuBar().Items().Append(helpItem);
    }

    void MainPage::ShowTimeSetting()
    {
        renderer.EnqueueTask([this]()
            {
                double julianDay = appCore.Simulation().JulianDay();
                Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, julianDay]()
                    {
                        ShowTimeSetting(julianDay);
                    });
            });
    }

    IAsyncAction MainPage::ShowTimeSetting(double julianDay)
    {
        TimeSettingDialog dialog{ julianDay };
        if (co_await ContentDialogHelper::ShowContentDialogAsync(*this, dialog) == ContentDialogResult::Primary)
        {
            auto newDay = dialog.JulianDay();
            renderer.EnqueueTask([this, newDay]()
                {
                    appCore.Simulation().JulianDay(newDay);
                });
        }
    }

    IAsyncAction MainPage::ShowGotoObject()
    {
        GotoObjectDialog dialog{ appCore, renderer };
        if (co_await ContentDialogHelper::ShowContentDialogAsync(*this, dialog) == ContentDialogResult::Primary)
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
                        Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this]
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

    IAsyncAction MainPage::ShowObjectNotFound()
    {
        co_await ContentDialogHelper::ShowAlert(*this, LocalizationHelper::Localize(L"Object not found", L""));
    }

    IAsyncAction MainPage::PickScript()
    {
        Pickers::FileOpenPicker picker;
        picker.ViewMode(Pickers::PickerViewMode::Thumbnail);
        picker.SuggestedStartLocation(Pickers::PickerLocationId::Downloads);
        picker.FileTypeFilter().Append(L".cel");
        picker.FileTypeFilter().Append(L".celx");
        auto file{ co_await picker.PickSingleFileAsync() };
        if (file == nullptr) co_return;

        OpenFileIfReady(file);
    }

    void MainPage::OpenFileIfReady(StorageFile const scriptFile)
    {
        Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, scriptFile]()
            {
                scriptFileToOpen = scriptFile;
                if (readyForInput)
                    OpenFileOrURL();
            });
    }

    void MainPage::OpenURLIfReady(Uri const url)
    {
        Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, url]()
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

    IAsyncAction MainPage::OpenFileOrURL()
    {
        // TODO
        co_return;
    }

    void MainPage::CopyURL()
    {
        using namespace Windows::ApplicationModel::DataTransfer;
        renderer.EnqueueTask([this]()
            {
                auto url = appCore.CurrentURL();
                Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, url]()
                    {
                        DataPackage dataPackage;
                        dataPackage.RequestedOperation(DataPackageOperation::Copy);
                        dataPackage.SetText(appCore.CurrentURL());
                        Clipboard::SetContent(dataPackage);
                    });
            });
    }

    IAsyncAction MainPage::PasteURL()
    {
        using namespace Windows::ApplicationModel::DataTransfer;
        auto dataPackageView = Clipboard::GetContent();
        if (dataPackageView.Contains(StandardDataFormats::Text()))
        {
            auto text = co_await dataPackageView.GetTextAsync();
            OpenURLIfReady(Uri(text));
        }
    }

    IAsyncAction MainPage::ShowSelectObject()
    {
        auto text = co_await ContentDialogHelper::GetText(*this, LocalizationHelper::Localize(L"Object name:", L""));
        if (text.empty()) co_return;

        renderer.EnqueueTask([this, text]()
            {
                auto selection = appCore.Simulation().Find(text);
                if (selection.IsEmpty())
                {
                    Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this]
                        {
                            ShowObjectNotFound();
                        });
                }
                else
                {
                    appCore.Simulation().Selection(selection);
                }
            });
    }

    void MainPage::CaptureImage()
    {
        auto tempFolder{ ApplicationData::Current().TemporaryFolder() };
        auto path = PathHelper::Combine(tempFolder.Path(), to_hstring(GuidHelper::CreateNewGuid()) + L".png");
        renderer.EnqueueTask([this, path]()
            {
                if (appCore.SaveScreenshot(path))
                {
                    Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, path]()
                        {
                            SaveScreenshot(path);
                        });
                }
                else
                {
                    Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this]()
                        {
                            ContentDialogHelper::ShowAlert(*this, LocalizationHelper::Localize(L"Failed in capturing screenshot.", L""));
                        });
                }
            });
    }

    IAsyncAction MainPage::SaveScreenshot(hstring const& path)
    {
        try
        {
            auto originalFile{ co_await StorageFile::GetFileFromPathAsync(path) };
            if (originalFile == nullptr) co_return;

            Pickers::FileSavePicker savePicker;
            savePicker.SuggestedStartLocation(Pickers::PickerLocationId::DocumentsLibrary);
            // Dropdown of file types the user can save the file as
            auto fileExtensions = winrt::single_threaded_observable_vector<winrt::hstring>();
            fileExtensions.Append(L".png");
            savePicker.FileTypeChoices().Insert(LocalizationHelper::Localize(L"Image", L"Sharing option, image"), fileExtensions);
            // Default file name if the user does not type one in or select a file to replace
            savePicker.SuggestedFileName(LocalizationHelper::Localize(L"Celestia Screenshot", L""));

            StorageFile file{ co_await savePicker.PickSaveFileAsync() };
            if (file == nullptr) co_return;

            co_await originalFile.CopyAndReplaceAsync(file);
        }
        catch (hresult_error const&)
        {
            ContentDialogHelper::ShowAlert(*this, LocalizationHelper::Localize(L"Failed in capturing screenshot.", L""));
        }
    }

    void MainPage::ShowTourGuide()
    {
        auto destinations = appCore.Destinations();
        auto list = single_threaded_vector<CelestiaDestination>({ destinations.begin(), destinations.end() });
        ShowPage(xaml_typename<CelestiaUWP2::TourGuidePage>(), Size(400, 0), CelestiaUWP2::TourGuideParameter(appCore, renderer, list));
    }

    void MainPage::ShowInfo(CelestiaSelection const& selection)
    {
        ShowPage(xaml_typename<CelestiaUWP2::InfoPage>(), Size(400, 0), CelestiaUWP2::InfoParameter(appCore, renderer, selection));
    }

    void MainPage::ShowEclipseFinder()
    {
        ShowPage(xaml_typename<CelestiaUWP2::EclipseFinderPage>(), Size(400, 0), CelestiaUWP2::EclipseFinderParameter(appCore, renderer));
    }

    fire_and_forget MainPage::ShowObserverMode()
    {
        ObserverModeDialog dialog{ appCore, renderer };
        if (co_await ContentDialogHelper::ShowContentDialogAsync(*this, dialog) != ContentDialogResult::Primary) co_return;
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

    void MainPage::ShowBrowser()
    {
        ShowPage(xaml_typename<CelestiaUWP2::BrowserPage>(), Size(400, 0), CelestiaUWP2::BrowserParameter(appCore, renderer));
    }

    void MainPage::ShowBookmarkOrganizer()
    {
        ShowPage(xaml_typename<CelestiaUWP2::BookmarkOrganizerPage>(), Size(400, 0), CelestiaUWP2::BookmarkOrganizerParameter(appCore, renderer, false));
    }

    void MainPage::ShowNewBookmark()
    {
        ShowPage(xaml_typename<CelestiaUWP2::NewBookmarkPage>(), Size(400, 0), CelestiaUWP2::NewBookmarkParameter(appCore, renderer));
    }

    void MainPage::ShowSettings()
    {
        auto languages = single_threaded_vector<hstring>();
        languages.ReplaceAll(availableLanguages);
        ShowPage(xaml_typename<CelestiaUWP2::SettingsPage>(), Size(400, 0), CelestiaUWP2::SettingsParameter(appCore, renderer, appSettings, ApplicationData::Current().LocalSettings(), languages));
    }

    void MainPage::ShowAddonManagement()
    {
        ShowPage(xaml_typename<CelestiaUWP2::ResourceManagerPage>(), Size(400, 0), CelestiaUWP2::ResourceManagerParameter(appCore, renderer, resourceManager, [weak_this{ get_weak() }]()
            {
                auto strong_this = weak_this.get();
                if (strong_this)
                    strong_this->ShowOnlineAddons();
            }));
    }

    fire_and_forget MainPage::ShowOnlineAddons()
    {
        std::map<hstring, hstring> content;
        content.emplace(L"lang", LocalizationHelper::Locale());
        auto encoder = Windows::Web::Http::HttpFormUrlEncodedContent(content);
        hstring url = hstring(L"https://celestia.mobi/resources/categories") + L"?" + encoder.ToString();
        co_await Launcher::LaunchUriAsync(Uri(url));
    }

    void MainPage::ToggleFullScreen()
    {
        auto view = Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
        if (view.IsFullScreenMode())
            view.ExitFullScreenMode();
        else
            view.TryEnterFullScreenMode();
    }

    fire_and_forget MainPage::ReportBug()
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
                    Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, screenshotFile, renderInfoFile, urlInfoFile, systemInfoFile, addonInfoFile, renderInfo, url, saveScreenshotSuccess]()
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

    fire_and_forget MainPage::ReportBug(StorageFile const screenshotFile, StorageFile const renderInfoFile, StorageFile const urlInfoFile, StorageFile const systemInfoFile, StorageFile const addonInfoFile, hstring const renderInfo, hstring const url)
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

            using namespace Windows::ApplicationModel::Email;
            EmailMessage emailMessage;
            emailMessage.To().Append(EmailRecipient(feedbackEmailAddress));
            emailMessage.Subject(LocalizationHelper::Localize(L"Bug report for Celestia", L"Default email title for bug report"));
            emailMessage.Body(LocalizationHelper::Localize(L"Please describe the issue and repro steps, if known.", L"Default email body for bug report"));
            if (screenshotFile != nullptr)
                emailMessage.Attachments().Append(EmailAttachment(screenshotFile.Name(), screenshotFile));
            emailMessage.Attachments().Append(EmailAttachment(renderInfoFile.Name(), renderInfoFile));
            emailMessage.Attachments().Append(EmailAttachment(urlInfoFile.Name(), urlInfoFile));
            emailMessage.Attachments().Append(EmailAttachment(systemInfoFile.Name(), systemInfoFile));
            if (!installedAddonList.empty())
                emailMessage.Attachments().Append(EmailAttachment(addonInfoFile.Name(), addonInfoFile));
            co_await EmailManager::ShowComposeNewEmailAsync(emailMessage);
        }
        catch (hresult_error const&)
        {
            ReportBugOrSuggestFeatureFallback();
        }
    }

    fire_and_forget MainPage::SuggestFeature()
    {
        try
        {
            using namespace Windows::ApplicationModel::Email;
            EmailMessage emailMessage;
            emailMessage.To().Append(EmailRecipient(feedbackEmailAddress));
            emailMessage.Subject(LocalizationHelper::Localize(L"Feature suggestion for Celestia", L"Default email title for feature suggestion"));
            emailMessage.Body(LocalizationHelper::Localize(L"Please describe the feature you want to see in Celestia.", L"Default email body for feature suggestion"));
            co_await EmailManager::ShowComposeNewEmailAsync(emailMessage);
        }
        catch (hresult_error const&)
        {
            ReportBugOrSuggestFeatureFallback();
        }
    }

    fire_and_forget MainPage::ReportBugOrSuggestFeatureFallback()
    {
        co_await Launcher::LaunchUriAsync(Uri(feedbackLinkAddress));
    }

    void MainPage::ShowHelp()
    {
        // TODO
    }

    void MainPage::AppCore_ShowContextMenu(IInspectable const&, ShowContextMenuArgs const& args)
    {
        auto x = args.X();
        auto y = args.Y();
        auto selection = args.Selection();
        Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, selection, x, y]()
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
                    for (const auto child : children)
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
                    AppendItem(menu, LocalizationHelper::Localize(L"Unmark", L"Unmark an object"), [weak_this{ get_weak() }, selection](IInspectable const&, RoutedEventArgs const&)
                        {
                            auto strong_this{ weak_this.get() };
                            if (strong_this == nullptr) return;
                            strong_this->renderer.EnqueueTask([strong_this, selection]()
                                {
                                    strong_this->appCore.Simulation().Universe().UnmarkSelection(selection);
                                });
                        });
                }
                else
                {
                    MenuFlyoutSubItem action;
                    action.Text(LocalizationHelper::Localize(L"Mark", L"Mark an object"));
                    std::vector<std::pair<hstring, CelestiaMarkerRepresentation>> markers =
                    {
                        {LocalizationHelper::Localize(L"Diamond", L"Marker"), CelestiaMarkerRepresentation::Diamond},
                        {LocalizationHelper::Localize(L"Triangle", L"Marker"), CelestiaMarkerRepresentation::Triangle},
                        {LocalizationHelper::Localize(L"Filled Square", L"Marker"), CelestiaMarkerRepresentation::FilledSquare},
                        {LocalizationHelper::Localize(L"Plus", L"Marker"), CelestiaMarkerRepresentation::Plus},
                        {LocalizationHelper::Localize(L"X", L"Marker"), CelestiaMarkerRepresentation::X},
                        {LocalizationHelper::Localize(L"Left Arrow", L"Marker"), CelestiaMarkerRepresentation::LeftArrow},
                        {LocalizationHelper::Localize(L"Right Arrow", L"Marker"), CelestiaMarkerRepresentation::RightArrow},
                        {LocalizationHelper::Localize(L"Up Arrow", L"Marker"), CelestiaMarkerRepresentation::UpArrow},
                        {LocalizationHelper::Localize(L"Down Arrow", L"Marker"), CelestiaMarkerRepresentation::DownArrow},
                        {LocalizationHelper::Localize(L"Circle", L"Marker"), CelestiaMarkerRepresentation::Circle},
                        {LocalizationHelper::Localize(L"Disk", L"Marker"), CelestiaMarkerRepresentation::Disk},
                        {LocalizationHelper::Localize(L"Crosshair", L"Marker"), CelestiaMarkerRepresentation::Crosshair}
                    };

                    for (const auto& [name, type] : markers)
                    {
                        auto copiedType = type;
                        AppendSubItem(action, name, [weak_this{ get_weak() }, selection, copiedType](IInspectable const&, RoutedEventArgs const&)
                            {
                                auto strong_this{ weak_this.get() };
                                if (strong_this == nullptr) return;
                                strong_this->renderer.EnqueueTask([strong_this, selection, copiedType]()
                                    {
                                        strong_this->appCore.Simulation().Universe().MarkSelection(selection, copiedType);
                                        strong_this->appCore.ShowMarkers(true);
                                    });
                            });
                    }
                    menu.Items().Append(action);
                }

                menu.ShowAt(GLView(), Point(x / scale, y / scale));
            });
    }

    void MainPage::AppCore_FatalError(IInspectable const&, FatalErrorArgs const& args)
    {
        auto message = args.Message();
        Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [weak_this{ get_weak() }, message]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this != nullptr)
                {
                    ContentDialogHelper::ShowAlert(*strong_this, message);
                }
            });
    }

    void MainPage::GLView_PointerPressed(IInspectable const& sender, Input::PointerRoutedEventArgs const& args)
    {
        if (args.Pointer().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Mouse)
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
                lastMouseGlobalPosition = Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerPosition();
                if (!isMouseHidden)
                {
                    isMouseHidden = true;
                    GLView().CapturePointer(args.Pointer());
                    Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerCursor(nullptr);
                }
            }
        }
    }

    void MainPage::GLView_PointerMoved(IInspectable const& sender, Input::PointerRoutedEventArgs const& args)
    {
        if (args.Pointer().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Mouse)
        {
            if (!lastMousePosition.has_value() || !currentPressedButton.has_value() || !lastMouseGlobalPosition.has_value()) return;

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
            Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerPosition(lastMouseGlobalPosition.value());
        }
    }

    void MainPage::GLView_PointerReleased(IInspectable const& sender, Input::PointerRoutedEventArgs const& args)
    {
        if (args.Pointer().PointerDeviceType() == Windows::Devices::Input::PointerDeviceType::Mouse)
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
                lastMouseGlobalPosition = std::nullopt;

                if (isMouseHidden)
                {
                    isMouseHidden = false;
                    GLView().ReleasePointerCapture(args.Pointer());
                    using namespace Windows::UI::Core;
                    CoreWindow::GetForCurrentThread().PointerCursor(CoreCursor(CoreCursorType::Arrow, 0));
                }
            }
        }
    }

    void MainPage::GLView_PointerWheelChanged(IInspectable const& sender, Input::PointerRoutedEventArgs const& args)
    {
        auto delta = args.GetCurrentPoint(sender.as<UIElement>()).Properties().MouseWheelDelta();
        renderer.EnqueueTask([this, delta]()
            {
                appCore.MouseWheel(delta > 0 ? -1.0f : 1.0f, 0);
            });
    }

    void MainPage::FocusManager_GotFocus(IInspectable const&, Input::FocusManagerGotFocusEventArgs const& args)
    {
        auto focusedItem = args.NewFocusedElement();
        auto glViewHasFocus = focusedItem == FocusHelperControl();
        if (!glViewHasFocus)
        {
            auto element = focusedItem.try_as<FrameworkElement>();
            if (element != nullptr)
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

    void MainPage::FocusHelperControl_CharacterReceived(IInspectable const&, Input::CharacterReceivedRoutedEventArgs const& args)
    {
        int16_t key = args.Character();
        renderer.EnqueueTask([this, key]()
            {
                appCore.CharEnter(key);
            });
    }

    void MainPage::FocusHelperControl_KeyDown(IInspectable const&, Input::KeyRoutedEventArgs const& args)
    {
        if (args.OriginalKey() >= VirtualKey::GamepadA && args.OriginalKey() <= VirtualKey::GamepadRightThumbstickLeft) return;

        int32_t modifiers = 0;
        auto key = args.Key();

        using namespace Windows::UI::Core;
        if (CoreWindow::GetForCurrentThread().GetKeyState(VirtualKey::Control) == CoreVirtualKeyStates::Down)
            modifiers |= 16;
        if (CoreWindow::GetForCurrentThread().GetKeyState(VirtualKey::Shift) == CoreVirtualKeyStates::Down)
            modifiers |= 8;

        renderer.EnqueueTask([this, key, modifiers]()
            {
                appCore.KeyDown(static_cast<int32_t>(key), modifiers);
            });
    }

    void MainPage::MenuBar_KeyUp(IInspectable const&, Input::KeyRoutedEventArgs const& args)
    {
        if (isXbox && args.Key() == VirtualKey::GamepadMenu && OverlayContainer().Content() == nullptr)
        {
            MenuBar().Visibility(Visibility::Collapsed);
            FocusHelperControl().Focus(FocusState::Programmatic);
        }
    }

    void MainPage::FocusHelperControl_KeyUp(IInspectable const&, Input::KeyRoutedEventArgs const& args)
    {
        if (OverlayContainer().Content() != nullptr) return;
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
                            sender.as<Microsoft::UI::Xaml::Controls::MenuBarItem>().Focus(FocusState::Programmatic);
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

    void MainPage::ClosePanelButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ClosePanel();
    }

    void MainPage::ClosePanel()
    {
        OverlayBackground().Visibility(Visibility::Collapsed);
        OverlayContainer().Content(nullptr);
        // enable focus on menubar
        for(auto const& item : MenuBar().Items())
            item.IsTabStop(true);
        // move focus to either menubar or GLView
        if (MenuBar().Visibility() == Visibility::Visible)
            MenuBar().Items().GetAt(0).Focus(FocusState::Programmatic);
        else
            FocusHelperControl().Focus(FocusState::Programmatic);
        renderer.EnqueueTask([this]()
            {
                hasOverlayOpen = false;
            });
    }

    void MainPage::ShowPage(Interop::TypeName const& pageType, Size const& size, IInspectable const& parameter)
    {
        OverlayBackground().Visibility(Visibility::Visible);
        OverlayBackground().Width(size.Width);
        if (size.Height <= 1)
        {
            OverlayContainer().Height(OverlayBackground().Height());
            RelativePanel::SetAlignBottomWithPanel(OverlayContainer(), true);
        }
        else
        {
            OverlayContainer().Height(size.Height);
            RelativePanel::SetAlignBottomWithPanel(OverlayContainer(), false);
        }
        OverlayContainer().Navigate(pageType, parameter);
        // Disable focus on menubar
        for(auto item : MenuBar().Items())
            item.IsTabStop(false);
        if (ClosePanelButton().IsLoaded())
            ClosePanelButton().Focus(FocusState::Programmatic);
        else
            ClosePanelButton().Loaded([](IInspectable const& sender, RoutedEventArgs const&)
                {
                    sender.as<Button>().Focus(FocusState::Programmatic);
                });
        renderer.EnqueueTask([this]()
            {
                hasOverlayOpen = true;
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

    void AppendItem(Microsoft::UI::Xaml::Controls::MenuBarItem const& parent, hstring const& text, RoutedEventHandler const& click, Input::KeyboardAccelerator const& accelerator)
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

    void AppendCharEnterItem(Microsoft::UI::Xaml::Controls::MenuBarItem const& item, hstring const& title, short input, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, std::optional<VirtualKey> key, std::optional<VirtualKeyModifiers> modifiers)
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

                    for (const auto& child : items)
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
