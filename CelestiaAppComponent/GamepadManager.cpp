// GamepadManager.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "GamepadManager.h"
#if __has_include("GamepadManager.g.cpp")
#include "GamepadManager.g.cpp"
#endif

using namespace std;
using namespace winrt;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Windows::Gaming::Input;

namespace winrt::CelestiaAppComponent::implementation
{
    GamepadManager::GamepadManager(CelestiaAppComponent::AppSettings const& appSettings) : appSettings(appSettings)
    {
        connectedGamepads = single_threaded_vector<Gamepad>();
        for (const auto& gamepad : Gamepad::Gamepads())
            connectedGamepads.Append(gamepad);

        Gamepad::GamepadAdded([this](IInspectable const&, Gamepad const& gamepad)
            {
                connectedGamepadMutex.lock();
                uint32_t index;
                if (!connectedGamepads.IndexOf(gamepad, index))
                    connectedGamepads.Append(gamepad);
                connectedGamepadMutex.unlock();
            });

        Gamepad::GamepadRemoved([this](IInspectable const&, Gamepad const& gamepad)
            {
                connectedGamepadMutex.lock();
                uint32_t index;
                if (connectedGamepads.IndexOf(gamepad, index))
                    connectedGamepads.RemoveAt(index);
                connectedGamepadMutex.unlock();
            });
    }

    void GamepadManager::GamepadButtonAction(CelestiaAppCore const& appCore, CelestiaComponent::CelestiaGamepadAction action, bool pressed)
    {
        CelestiaExtension::InvokeCelestiaGamepadAction(appCore, action, !pressed);
    }

    void GamepadManager::GamepadJoystickAction(CelestiaAppCore const& appCore, CelestiaJoystickAxis axis, double amount)
    {
        appCore.JoystickAxis(axis, (float)amount);
    }

    void GamepadManager::UpdateGamepad()
    {
        // Update the current gamepad if necessary.
        if (currentGamepad == nullptr)
        {
            currentGamepad = GetFirstGamepad();
        }
    }

    void GamepadManager::PollGamepad(CelestiaAppCore const& appCore)
    {
        UpdateGamepad();
        auto gamepad = currentGamepad;
        if (gamepad == nullptr) return;

        auto reading = gamepad.GetCurrentReading();
        bool isAPressedNew = (int)(reading.Buttons & GamepadButtons::A) != 0;
        bool isBPressedNew = (int)(reading.Buttons & GamepadButtons::B) != 0;
        bool isXPressedNew = (int)(reading.Buttons & GamepadButtons::X) != 0;
        bool isYPressedNew = (int)(reading.Buttons & GamepadButtons::Y) != 0;
        bool isLeftTriggerPressedNew = reading.LeftTrigger >= 0.5;
        bool isRightTriggerPressedNew = reading.RightTrigger >= 0.5;
        bool isLeftBumperPressedNew = (int)(reading.Buttons & GamepadButtons::LeftShoulder) != 0;
        bool isRightBumperPressedNew = (int)(reading.Buttons & GamepadButtons::RightShoulder) != 0;
        bool isDpadUpPressedNew = (int)(reading.Buttons & GamepadButtons::DPadUp) != 0;
        bool isDpadDownPressedNew = (int)(reading.Buttons & GamepadButtons::DPadDown) != 0;
        bool isDpadLeftPressedNew = (int)(reading.Buttons & GamepadButtons::DPadLeft) != 0;
        bool isDpadRightPressedNew = (int)(reading.Buttons & GamepadButtons::DPadRight) != 0;
        double thumbstickXLeft = 0.0;
        double thumbstickYLeft = 0.0;
        double thumbstickXRight = 0.0;
        double thumbstickYRight = 0.0;
        if (appSettings.GamepadEnableLeftThumbstick())
        {
            thumbstickXLeft = reading.LeftThumbstickX;
            thumbstickYLeft = reading.LeftThumbstickY;
        }
        if (appSettings.GamepadEnableRightThumbstick())
        {
            thumbstickXRight = reading.RightThumbstickX;
            thumbstickYRight = reading.RightThumbstickY;
        }
        if (isAPressedNew != isAPressed)
        {
            isAPressed = isAPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapA(), isAPressed);
        }
        if (isBPressedNew != isBPressed)
        {
            isBPressed = isBPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapB(), isBPressed);
        }
        if (isXPressedNew != isXPressed)
        {
            isXPressed = isXPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapX(), isXPressed);
        }
        if (isYPressedNew != isYPressed)
        {
            isYPressed = isYPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapY(), isYPressed);
        }
        if (isLeftTriggerPressedNew != isLeftTriggerPressed)
        {
            isLeftTriggerPressed = isLeftTriggerPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapLT(), isLeftTriggerPressed);
        }
        if (isRightTriggerPressedNew != isRightTriggerPressed)
        {
            isRightTriggerPressed = isRightTriggerPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapRT(), isRightTriggerPressed);
        }
        if (isLeftBumperPressedNew != isLeftBumperPressed)
        {
            isLeftBumperPressed = isLeftBumperPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapLB(), isLeftBumperPressed);
        }
        if (isRightBumperPressedNew != isRightBumperPressed)
        {
            isRightBumperPressed = isRightBumperPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapRB(), isRightBumperPressed);
        }
        if (isDpadUpPressedNew != isDpadUpPressed)
        {
            isDpadUpPressed = isDpadUpPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapDpadUp(), isDpadUpPressed);
        }
        if (isDpadDownPressedNew != isDpadDownPressed)
        {
            isDpadDownPressed = isDpadDownPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapDpadDown(), isDpadDownPressed);
        }
        if (isDpadLeftPressedNew != isDpadLeftPressed)
        {
            isDpadLeftPressed = isDpadLeftPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapDpadLeft(), isDpadLeftPressed);
        }
        if (isDpadRightPressedNew != isDpadRightPressed)
        {
            isDpadRightPressed = isDpadRightPressedNew;
            GamepadButtonAction(appCore, appSettings.GamepadRemapDpadRight(), isDpadRightPressed);
        }
        if (appSettings.GamepadInvertX())
        {
            thumbstickXLeft = -thumbstickXLeft;
            thumbstickXRight = -thumbstickXRight;
        }
        if (appSettings.GamepadInvertY())
        {
            thumbstickYLeft = -thumbstickYLeft;
            thumbstickYRight = -thumbstickYRight;
        }
        if (thumbstickXLeft != 0.0)
        {
            GamepadJoystickAction(appCore, CelestiaJoystickAxis::X, thumbstickXLeft);
        }
        if (thumbstickYLeft != 0.0)
        {
            GamepadJoystickAction(appCore, CelestiaJoystickAxis::Y, thumbstickYLeft);
        }
        if (thumbstickXRight != 0.0)
        {
            GamepadJoystickAction(appCore, CelestiaJoystickAxis::RX, thumbstickXRight);
        }
        if (thumbstickYRight != 0.0)
        {
            GamepadJoystickAction(appCore, CelestiaJoystickAxis::RY, thumbstickYRight);
        }
    }

    Gamepad GamepadManager::GetFirstGamepad()
    {
        Gamepad gamepad = nullptr;
        connectedGamepadMutex.lock();
        if (connectedGamepads.Size() > 0)
        {
            gamepad = connectedGamepads.GetAt(0);
        }
        connectedGamepadMutex.unlock();
        return gamepad;
    }
}
