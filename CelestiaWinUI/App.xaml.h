// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "App.xaml.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct App : AppT<App>
    {
        App();

        fire_and_forget OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const);
        void OnFileActivated(Windows::ApplicationModel::Activation::FileActivatedEventArgs const&);
        void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const&);
        void MainInstance_Activated(Windows::Foundation::IInspectable const&, Microsoft::Windows::AppLifecycle::AppActivationArguments const&);

    private:
        CelestiaWinUI::MainWindow window{ nullptr };
        void Launch(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&, Windows::ApplicationModel::Activation::IActivatedEventArgs const&);
    };
}
