// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        winrt::fire_and_forget MainWindow_Loaded();

        CelestiaAppComponent::AppSettings AppSettings();

        void OpenFileIfReady(Windows::Storage::StorageFile const& scriptFileToOpen);
        void OpenURLIfReady(Windows::Foundation::Uri const& urlToOpen);

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
        hstring extraAddonFolderPath{ L"" };
        hstring extraScriptFolder{ L"" };
        std::vector<hstring> availableLanguages;
        bool isMouseHidden{ false };
        std::optional<Windows::Foundation::Point> lastMousePosition{ std::nullopt };
        std::optional<Windows::Foundation::Point> lastMouseGlobalPosition{ std::nullopt };
        std::optional<CelestiaComponent::CelestiaMouseButton> currentPressedButton{ std::nullopt };
        Windows::Storage::StorageFile scriptFileToOpen{ nullptr };
        Windows::Foundation::Uri urlToOpen{ nullptr };
        bool isXbox{ false };
        bool isGLViewFocused{ false };
        bool didShowXboxWelcomeMessage{ false };

        bool StartEngine(hstring const& resourcePath, hstring const& configPath, hstring const& locale, Windows::Data::Json::JsonObject const& defaultSettings);
        Windows::Foundation::IAsyncAction CreateExtraFolders();
        void ShowLoadingFailure();
        void UpdateScale();
        void SetUpGLViewInteractions();
        void PopulateMenuBar(hstring const& resourcePath);
        void ShowTimeSetting();
        Windows::Foundation::IAsyncAction ShowTimeSetting(Windows::Foundation::DateTime const& original);
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
        winrt::fire_and_forget MainWindow::ShowObserverMode();
        void ShowBrowser();
        void ShowBookmarkOrganizer();
        void ShowNewBookmark();
        Windows::Foundation::IAsyncOperation<Windows::Data::Json::JsonObject> ReadDefaultSettings();
        void ApplySettings(Windows::Data::Json::JsonObject const& defaultSettings);
        void ShowSettings();
        void ShowAddonManagement();
        void ToggleFullScreen();

        void AppCore_ShowContextMenu(Windows::Foundation::IInspectable const&, CelestiaComponent::ShowContextMenuArgs const& args);
        void AppCore_FatalError(Windows::Foundation::IInspectable const&, CelestiaComponent::FatalErrorArgs const& args);
        void GLView_PointerPressed(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void GLView_PointerMoved(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void GLView_PointerReleased(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void GLView_PointerWheelChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void FocusManager_GotFocus(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Input::FocusManagerGotFocusEventArgs const& args);
        void FocusHelperControl_CharacterReceived(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Input::CharacterReceivedRoutedEventArgs const& args);
        void FocusHelperControl_KeyDown(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args);
        void FocusHelperControl_KeyUp(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args);
        void MenuBar_KeyUp(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& args);

        Windows::Foundation::IAsyncOperation<hstring> GetLocale(hstring const& localePath);
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}