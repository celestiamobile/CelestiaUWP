// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "MainWindow.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        void ClosePanelButton_Click(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::RoutedEventArgs const&);

        winrt::fire_and_forget MainWindow_Loaded();
        void MainWindow_SizeChanged(winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::WindowSizeChangedEventArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore{ nullptr };
        CelestiaComponent::CelestiaRenderer renderer{ nullptr };
        float scale{ 1.0f };
        bool readyForInput{ false };
        hstring defaultParentPath{ L""};
        hstring defaultResourcePath{ L"" };
        hstring defaultConfigFilePath{ L"" };
        hstring extraAddonFolder{ L"" };
        hstring extraScriptFolder{ L"" };

        bool StartEngine(hstring const& resourcePath, hstring const& configPath, hstring const& locale);
        Windows::Foundation::IAsyncAction CreateExtraFolders();
        void ShowLoadingFailure();
        void UpdateScale();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
