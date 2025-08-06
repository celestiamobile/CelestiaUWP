// App.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

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
