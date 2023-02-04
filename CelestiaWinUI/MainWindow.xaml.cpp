// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace CelestiaComponent;
using namespace Windows::Storage;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::CelestiaWinUI::implementation
{
    MainWindow::MainWindow()
    {
        CelestiaAppCore::SetUpLocale();
        appCore = CelestiaAppCore();
        InitializeComponent();

        defaultParentPath = Windows::ApplicationModel::Package::Current().InstalledLocation().Path();
        defaultResourcePath = defaultParentPath + L"\\CelestiaResources";
        defaultConfigFilePath = defaultResourcePath + L"\\celestia.cfg";

        MainWindow_Loaded();
    }

    void MainWindow::ClosePanelButton_Click(IInspectable const&, RoutedEventArgs const&)
    {

    }

    fire_and_forget MainWindow::MainWindow_Loaded()
    {
        // scale = AppSettings.UseFullDPI ? ((int)Windows.Graphics.Display.DisplayInformation.GetForCurrentView().ResolutionScale) / 100.0f : 1.0f;

        StorageFolder customDataFolder{ nullptr };
        StorageFile customConfigFile{ nullptr };
        try
        {
            customDataFolder = co_await ApplicationData::Current().LocalFolder().GetFolderAsync(L"Override");
            customConfigFile = co_await customDataFolder.GetFileAsync(L"celestia.cfg");
        }
        catch (hresult_error const&) { }

        auto resourcePath = customDataFolder != nullptr ? customDataFolder.Path() : defaultResourcePath;
        auto configPath = customConfigFile != nullptr ? customConfigFile.Path() : defaultConfigFilePath;

        auto localePath = defaultResourcePath + L"\\locale";
        // auto systemLocale = co_await GetLocale(localePath);
        // var locale = AppSettings.LanguageOverride;
        // if (locale == null)
        //    locale = systemLocale;
        hstring locale = L"en";

        co_await CreateExtraFolders();

        renderer = CelestiaRenderer(false, [this, resourcePath, configPath, locale]()
        {
            return StartEngine(resourcePath, configPath, locale);
        });
        renderer.SetCorePointer(appCore.Pointer());
        renderer.SetSurface(GLView(), scale);
        GLView().SizeChanged([this](IInspectable const&, SizeChangedEventArgs const& arg)
        {
            renderer.SetSize((int32_t)arg.NewSize().Width, (int32_t)arg.NewSize().Height);
        });
        renderer.Start();
    }

    void MainWindow::MainWindow_SizeChanged(IInspectable const&, WindowSizeChangedEventArgs const&)
    {
    }

    bool MainWindow::StartEngine(hstring const& resourcePath, hstring const& configPath, hstring const& locale)
    {
        CelestiaAppCore::InitGL();

        std::vector<hstring> extraPaths;
        if (extraAddonFolder.empty())
            extraPaths.push_back(extraAddonFolder);

        SetCurrentDirectoryW(resourcePath.c_str());
        hstring localeDirectory = resourcePath + L"\\locale";
        CelestiaAppCore::SetLocaleDirectory(localeDirectory, locale);

        bool loadSuccess = appCore.StartSimulation(configPath, extraPaths, [this](hstring const& status) {
            DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, status]() {
                LoadingText().Text(status);
                });
            }
        );
        if (!loadSuccess) return false;

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

        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this]()
        {
            LoadingText().Visibility(Visibility::Collapsed);
        //Addon.ResourceManager.Shared.AddonFolderPath = mExtraAddonFolder;
        //SetUpGLViewInteractions();
        //PopulateMenuBar(resourcePath);
            renderer.SetSize(GLView().ActualWidth(), GLView().ActualHeight());
        });

        //ApplySettings(ReadSettings().Result);

        appCore.Start();

        readyForInput = true;

        DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this]()
        {
        //    OpenFileOrURL();
        });
        return true;
    }

    Windows::Foundation::IAsyncAction MainWindow::CreateExtraFolders()
    {
        auto folder{ ApplicationData::Current().LocalFolder() };
        try
        {
            auto mainFolder = co_await folder.CreateFolderAsync(L"CelestiaResources", CreationCollisionOption::OpenIfExists);
            auto addonFolder = co_await mainFolder.CreateFolderAsync(L"extras", CreationCollisionOption::OpenIfExists);
            extraAddonFolder = addonFolder.Path();
            auto scriptFolder = co_await mainFolder.CreateFolderAsync(L"scripts", CreationCollisionOption::OpenIfExists);
            extraScriptFolder = scriptFolder.Path();
        }
        catch (hresult_error const&) { }
    }

    void MainWindow::ShowLoadingFailure()
    {
        LoadingText().Text(LocalizationHelper::Localize(L"Loading Celestia failed…"));
    }

    void MainWindow::UpdateScale()
    {
        appCore.SetDPI((int)(96.0f * scale));
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

        auto pathPrefix = defaultResourcePath + L"\\fonts\\";
        appCore.ClearFonts();
        appCore.SetFont(pathPrefix + regularFont, regularFontIndex, 9);
        appCore.SetTitleFont(pathPrefix + boldFont, boldFontIndex, 15);
        appCore.SetRenderFont(pathPrefix + regularFont, regularFontIndex, 9, CelestiaFontStyle::normal);
        appCore.SetRenderFont(pathPrefix + boldFont, boldFontIndex, 15, CelestiaFontStyle::large);
    }
}