// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Microsoft::Windows::AppLifecycle;
using namespace CelestiaWinUI;
using namespace CelestiaWinUI::implementation;

App::App()
{
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