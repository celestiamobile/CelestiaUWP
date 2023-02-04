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
    InitializeComponent();

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
    Launch(args, nullptr, nullptr);
}

void App::OnFileActivated(Windows::ApplicationModel::Activation::FileActivatedEventArgs const& args)
{
    Launch(nullptr, args, nullptr);
}

void App::OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const& args)
{
    Launch(nullptr, nullptr, args);
}

void App::MainInstance_Activated(IInspectable const&, Microsoft::Windows::AppLifecycle::AppActivationArguments const& args)
{
    if (args.Kind() == ExtendedActivationKind::File)
    {
        auto fileArgs = args.Data().as<Windows::ApplicationModel::Activation::FileActivatedEventArgs>();
        Launch(nullptr, fileArgs, nullptr);
    }
    else if (args.Kind() == ExtendedActivationKind::Protocol)
    {
        auto urlArgs = args.Data().as<Windows::ApplicationModel::Activation::ProtocolActivatedEventArgs>();
        Launch(nullptr, nullptr, urlArgs);
    }
}

void App::Launch(LaunchActivatedEventArgs const&, Windows::ApplicationModel::Activation::FileActivatedEventArgs const& fileEvent, Windows::ApplicationModel::Activation::IActivatedEventArgs const& urlEvent)
{
    if (window == nullptr) return;
    if (fileEvent != nullptr && fileEvent.Files() != nullptr && fileEvent.Files().Size() > 0)
    {
        auto file = fileEvent.Files().GetAt(0).try_as<Windows::Storage::StorageFile>();
        if (file != nullptr)
        {
            window.OpenFileIfReady(file);
        }
    }
    else if (urlEvent != nullptr && urlEvent.Kind() == Windows::ApplicationModel::Activation::ActivationKind::Protocol)
    {
        auto e = urlEvent.as<Windows::ApplicationModel::Activation::ProtocolActivatedEventArgs>();
        try
        {
            window.OpenURLIfReady(e.Uri());
        }
        catch (hresult_error const&) { };
    }
    window.Activate();
}