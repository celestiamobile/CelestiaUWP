//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"

#include "App.h"

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace CelestiaUWP2;
using namespace CelestiaUWP2::implementation;

App::App()
{
    Suspending({ this, &App::OnSuspending });

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

    bool isXbox = Windows::System::Profile::AnalyticsInfo::VersionInfo().DeviceFamily() == L"Windows.Xbox";
    if (isXbox)
        RequiresPointerMode(ApplicationRequiresPointerMode::WhenRequested);
    SetEnvironmentVariableW(L"WEBVIEW2_DEFAULT_BACKGROUND_COLOR", L"0");
}

void App::OnLaunched(LaunchActivatedEventArgs const& e)
{
    Launch(e, nullptr);
}

void App::OnFileActivated(FileActivatedEventArgs const& e)
{
    Launch(nullptr, e);
}
void App::OnActivated(IActivatedEventArgs const& e)
{
    Launch(nullptr, e);
}

void App::Launch(LaunchActivatedEventArgs const& launchEvent, IActivatedEventArgs const& activatedEvent)
{
    Frame rootFrame{ nullptr };
    auto content = Window::Current().Content();
    if (auto frame = content.try_as<Frame>(); frame != nullptr)
    {
        rootFrame = frame;
    }

    auto args = activatedEvent;
    if (args == nullptr)
        args = AppInstance::GetActivatedEventArgs();

    // Do not repeat app initialization when the Window already has content,
    // just ensure that the window is active
    if (rootFrame == nullptr)
    {
        // Create a Frame to act as the navigation context and associate it with
        // a SuspensionManager key
        rootFrame = Frame();

        rootFrame.NavigationFailed({ this, &App::OnNavigationFailed });

        // Place the frame in the current Window
        Window::Current().Content(rootFrame);
    }

    if (launchEvent == nullptr || launchEvent.PrelaunchActivated() == false)
    {
        auto page = rootFrame.Content().try_as<CelestiaUWP2::MainPage>();
        if (page == nullptr)
            rootFrame.Navigate(xaml_typename<CelestiaUWP2::MainPage>(), args);
        else
            page.WillActivate(args);
    }

    // Ensure the current window is active
    Window::Current().Activate();
}

void App::OnSuspending([[maybe_unused]] IInspectable const& sender, [[maybe_unused]] SuspendingEventArgs const& e)
{
    // Save application state and stop any background activity
}

void App::OnNavigationFailed(IInspectable const&, NavigationFailedEventArgs const& e)
{
    throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}