// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

#if defined(_M_IX86) || defined(_M_X64)
#define SUPPORTS_SENTRY
#include <sentry.h>
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Microsoft::Windows::AppLifecycle;
using namespace CelestiaAppComponent;
using namespace CelestiaWinUI;
using namespace CelestiaWinUI::implementation;

App::App()
{
#ifdef SUPPORTS_SENTRY
    auto installedLocation = Windows::ApplicationModel::Package::Current().InstalledLocation().Path();
    auto crashpadHandlerPath = PathHelper::Combine(installedLocation, L"crashpad_handler.exe");
    auto localFolder = Windows::Storage::ApplicationData::Current().LocalFolder().Path();
    auto sentryDatabasePath = PathHelper::Combine(localFolder, L"sentry");

    sentry_options_t* options = sentry_options_new();
    sentry_options_set_dsn(options, "SENTRY-DSN");
    sentry_options_set_handler_path(options, to_string(crashpadHandlerPath).c_str());
    sentry_options_set_database_path(options, to_string(sentryDatabasePath).c_str());
    sentry_options_set_release(options, "celestia-windows@2.1.1");
    sentry_init(options);
#endif

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
    });
#endif
}

fire_and_forget App::OnLaunched(LaunchActivatedEventArgs const args)
{
    auto mainInstance = AppInstance::FindOrRegisterForKey(L"Main");
    if (!mainInstance.IsCurrent())
    {
        co_await mainInstance.RedirectActivationToAsync(AppInstance::GetCurrent().GetActivatedEventArgs());
        Application::Current().Exit();
        co_return;
    }
    else
    {
        mainInstance.Activated({ get_weak(), &App::MainInstance_Activated });
    }

    window = make<MainWindow>();
    window.Closed([](IInspectable const&, WindowEventArgs const&)
        {
#ifdef SUPPORTS_SENTRY
            sentry_close();
#endif
            Application::Current().Exit();
        });
    Launch(args, nullptr);
}

void App::OnFileActivated(Windows::ApplicationModel::Activation::FileActivatedEventArgs const& args)
{
    Launch(nullptr, args);
}

void App::OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const& args)
{
    Launch(nullptr, args);
}

void App::MainInstance_Activated(IInspectable const&, Microsoft::Windows::AppLifecycle::AppActivationArguments const& args)
{
    if (auto event = args.Data().try_as<Windows::ApplicationModel::Activation::IActivatedEventArgs>(); event != nullptr)
        Launch(nullptr, event);
}

void App::Launch(LaunchActivatedEventArgs const&, Windows::ApplicationModel::Activation::IActivatedEventArgs const& event)
{
    if (window == nullptr) return;
    auto args = event;
    if (args == nullptr)
    {
        auto launchArgs = AppInstance::GetCurrent().GetActivatedEventArgs();
        if (launchArgs != nullptr)
        {
            args = launchArgs.Data().try_as<Windows::ApplicationModel::Activation::IActivatedEventArgs>();
        }
    }
    window.WillActivate(args);
    window.Activate();
}