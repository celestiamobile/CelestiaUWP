//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "MainPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
        void InitializeComponent();

        void WillActivate(Windows::ApplicationModel::Activation::IActivatedEventArgs const& args);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);
        winrt::fire_and_forget MainPage_Loaded(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);
        void MainPage_SizeChanged(IInspectable const& sender, Windows::UI::Xaml::SizeChangedEventArgs const& e);
        void ClosePanelButton_Click(IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        CelestiaComponent::CelestiaAppCore appCore{ nullptr };
        CelestiaComponent::CelestiaRenderer renderer{ nullptr };
        CelestiaAppComponent::AppSettings appSettings{ nullptr };
        CelestiaAppComponent::GamepadManager gamepadManager{ nullptr };
        CelestiaAppComponent::ResourceManager resourceManager{ nullptr };
        float scale{ 1.0f };
        bool readyForInput{ false };
        hstring defaultParentPath{ L""};
        hstring defaultResourcePath{ L"" };
        hstring defaultConfigFilePath{ L"" };
        Windows::Storage::StorageFolder extraAddonFolder{ nullptr };
        Windows::Storage::StorageFolder extraScriptFolder{ nullptr };
        hstring extraAddonFolderPath{ L"" };
        hstring extraScriptFolderPath{ L"" };
        std::vector<hstring> availableLanguages;
        bool isMouseHidden{ false };
        std::optional<Windows::Foundation::Point> lastMousePosition{ std::nullopt };
        std::optional<Windows::Foundation::Point> lastMouseGlobalPosition{ std::nullopt };
        std::optional<CelestiaComponent::CelestiaMouseButton> currentPressedButton{ std::nullopt };
        Windows::Storage::StorageFile scriptFileToOpen{ nullptr };
        Windows::Foundation::Uri urlToOpen{ nullptr };
        bool isXbox{ false };
        bool isGLViewFocused{ false };
        bool hasOverlayOpen{ false };

        bool StartEngine(hstring const& resourcePath, hstring const& configPath, hstring const& locale, CelestiaComponent::CelestiaLayoutDirection layoutDirection, Windows::Data::Json::JsonObject const& defaultSettings);
        Windows::Foundation::IAsyncAction CreateExtraFolders();
        void ShowLoadingFailure();
        void UpdateScale();
        void SetUpGLViewInteractions();
        void PopulateMenuBar(hstring const& resourcePath);
        void ShowTimeSetting();
        Windows::Foundation::IAsyncAction ShowTimeSetting(double julianDay);
        Windows::Foundation::IAsyncAction ShowGotoObject();
        Windows::Foundation::IAsyncAction ShowObjectNotFound();
        Windows::Foundation::IAsyncAction PickScript();
        Windows::Foundation::IAsyncAction OpenFileOrURL();
        void CopyURL();
        Windows::Foundation::IAsyncAction PasteURL();
        Windows::Foundation::IAsyncAction ShowSelectObject();
        void CaptureImage();
        Windows::Foundation::IAsyncAction SaveScreenshot(hstring const& path);
        void ShowTourGuide();
        void ShowInfo(CelestiaComponent::CelestiaSelection const& selection);
        void ShowEclipseFinder();
        winrt::fire_and_forget ShowObserverMode();
        void ShowBrowser();
        void ShowBookmarkOrganizer();
        void ShowNewBookmark();
        Windows::Foundation::IAsyncOperation<Windows::Data::Json::JsonObject> ReadDefaultSettings();
        void ApplySettings(Windows::Data::Json::JsonObject const& defaultSettings);
        void ShowSettings();
        void ShowAddonManagement();
        winrt::fire_and_forget ShowOnlineAddons();
        void ToggleFullScreen();
        void ShowHelp();
        fire_and_forget ReportBug();
        fire_and_forget SuggestFeature();
        fire_and_forget ReportBugOrSuggestFeatureFallback();
        fire_and_forget ReportBug(Windows::Storage::StorageFile const screenshotFile, Windows::Storage::StorageFile const renderInfoFile, Windows::Storage::StorageFile const urlInfoFile, Windows::Storage::StorageFile const systemInfoFile, Windows::Storage::StorageFile const addonInfoFile, hstring const renderInfo, hstring const url);
        void OpenFileIfReady(Windows::Storage::StorageFile const scriptFileToOpen);
        void OpenURLIfReady(Windows::Foundation::Uri const urlToOpen);
        Windows::Foundation::IAsyncOperation<hstring> GetLocale(hstring const& localePath);
        void ClosePanel();
        void ShowPage(Windows::UI::Xaml::Interop::TypeName const& pageType, Windows::Foundation::Size const& size, Windows::Foundation::IInspectable const& parameter);

        void AppCore_ShowContextMenu(Windows::Foundation::IInspectable const&, CelestiaComponent::ShowContextMenuArgs const& args);
        void AppCore_FatalError(Windows::Foundation::IInspectable const&, CelestiaComponent::FatalErrorArgs const& args);
        void GLView_PointerPressed(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void GLView_PointerMoved(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void GLView_PointerReleased(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void GLView_PointerWheelChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void FocusManager_GotFocus(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::FocusManagerGotFocusEventArgs const& args);
        void FocusHelperControl_CharacterReceived(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::CharacterReceivedRoutedEventArgs const& args);
        void FocusHelperControl_KeyDown(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::KeyRoutedEventArgs const& args);
        void FocusHelperControl_KeyUp(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::KeyRoutedEventArgs const& args);
        void MenuBar_KeyUp(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::Input::KeyRoutedEventArgs const& args);
        void SystemNavigationManager_BackRequested(Windows::Foundation::IInspectable const&, Windows::UI::Core::BackRequestedEventArgs const&);
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
