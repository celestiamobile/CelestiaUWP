#pragma once

#include "GamepadManager.g.h"
#include <mutex>

namespace winrt::CelestiaAppComponent::implementation
{
    struct GamepadManager : GamepadManagerT<GamepadManager>
    {
        GamepadManager(CelestiaAppComponent::AppSettings const& appSettings);
        void PollGamepad(CelestiaComponent::CelestiaAppCore const& appCore);
    private:
        CelestiaAppComponent::AppSettings appSettings;
        Windows::Gaming::Input::Gamepad currentGamepad{ nullptr };
        Windows::Foundation::Collections::IVector<Windows::Gaming::Input::Gamepad> connectedGamepads;
        bool isAPressed{ false };
        bool isBPressed{ false };
        bool isXPressed{ false };
        bool isYPressed{ false };
        bool isLeftBumperPressed{ false };
        bool isRightBumperPressed{ false };
        bool isLeftTriggerPressed{ false };
        bool isRightTriggerPressed{ false };
        bool isDpadUpPressed{ false };
        bool isDpadDownPressed{ false };
        bool isDpadLeftPressed{ false };
        bool isDpadRightPressed{ false };
        std::mutex connectedGamepadMutex;

        void GamepadButtonAction(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaGamepadAction action, bool pressed);
        void GamepadJoystickAction(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaJoystickAxis axis, double amount);
        void UpdateGamepad();
        Windows::Gaming::Input::Gamepad GetFirstGamepad();
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct GamepadManager : GamepadManagerT<GamepadManager, implementation::GamepadManager>
    {
    };
}
