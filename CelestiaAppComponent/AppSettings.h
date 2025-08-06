// AppSettings.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "AppSettings.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct AppSettings : AppSettingsT<AppSettings>
    {
        AppSettings(Windows::Storage::ApplicationDataContainer const& settings);

        bool UseFullDPI();
        void UseFullDPI(bool);

        bool EnableMSAA();
        void EnableMSAA(bool);

        bool IgnoreXboxWelcomeMessage();
        void IgnoreXboxWelcomeMessage(bool);

        bool OnboardMessageDisplayed();
        void OnboardMessageDisplayed(bool);

        hstring LanguageOverride();
        void LanguageOverride(hstring const&);

        hstring LastNewsID();
        void LastNewsID(hstring const&);

        double PickSensitivity();
        void PickSensitivity(double);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapA();
        void GamepadRemapA(CelestiaComponent::CelestiaGamepadAction);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapB();
        void GamepadRemapB(CelestiaComponent::CelestiaGamepadAction);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapX();
        void GamepadRemapX(CelestiaComponent::CelestiaGamepadAction);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapY();
        void GamepadRemapY(CelestiaComponent::CelestiaGamepadAction);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapLT();
        void GamepadRemapLT(CelestiaComponent::CelestiaGamepadAction);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapRT();
        void GamepadRemapRT(CelestiaComponent::CelestiaGamepadAction);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapLB();
        void GamepadRemapLB(CelestiaComponent::CelestiaGamepadAction);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapRB();
        void GamepadRemapRB(CelestiaComponent::CelestiaGamepadAction);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapDpadUp();
        void GamepadRemapDpadUp(CelestiaComponent::CelestiaGamepadAction);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapDpadDown();
        void GamepadRemapDpadDown(CelestiaComponent::CelestiaGamepadAction);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapDpadLeft();
        void GamepadRemapDpadLeft(CelestiaComponent::CelestiaGamepadAction);

        CelestiaComponent::CelestiaGamepadAction GamepadRemapDpadRight();
        void GamepadRemapDpadRight(CelestiaComponent::CelestiaGamepadAction);

        bool GamepadInvertX();
        void GamepadInvertX(bool);

        bool GamepadInvertY();
        void GamepadInvertY(bool);

        bool GamepadEnableLeftThumbstick();
        void GamepadEnableLeftThumbstick(bool);

        bool GamepadEnableRightThumbstick();
        void GamepadEnableRightThumbstick(bool);

        void SetBoolean(CelestiaAppComponent::AppSettingBooleanEntry entry, bool value);
        bool GetBoolean(CelestiaAppComponent::AppSettingBooleanEntry entry);
        void SetInt32(CelestiaAppComponent::AppSettingInt32Entry entry, int32_t value);
        int32_t GetInt32(CelestiaAppComponent::AppSettingInt32Entry entry);
        void SetString(CelestiaAppComponent::AppSettingStringEntry entry, hstring const& value);
        hstring GetString(CelestiaAppComponent::AppSettingStringEntry entry);
        void SetDouble(CelestiaAppComponent::AppSettingDoubleEntry entry, double value);
        double GetDouble(CelestiaAppComponent::AppSettingDoubleEntry entry);

        void Save(Windows::Storage::ApplicationDataContainer const& settings);

    private:
        bool useFullDPI{ true };
        bool enableMSAA{ false };
        bool ignoreXboxWelcomeMessage{ false };
        bool onboardMessageDisplayed{ false };
        hstring languageOverride{ L"" };
        hstring lastNewsID{ L"" };
        double pickSensitivity{ 4.0 };

        CelestiaComponent::CelestiaGamepadAction gamepadRemapA{ CelestiaComponent::CelestiaGamepadAction::MoveSlower };
        CelestiaComponent::CelestiaGamepadAction gamepadRemapB{ CelestiaComponent::CelestiaGamepadAction::None };
        CelestiaComponent::CelestiaGamepadAction gamepadRemapX{ CelestiaComponent::CelestiaGamepadAction::MoveFaster };
        CelestiaComponent::CelestiaGamepadAction gamepadRemapY{ CelestiaComponent::CelestiaGamepadAction::None };
        CelestiaComponent::CelestiaGamepadAction gamepadRemapLT{ CelestiaComponent::CelestiaGamepadAction::RollLeft };
        CelestiaComponent::CelestiaGamepadAction gamepadRemapRT{ CelestiaComponent::CelestiaGamepadAction::RollRight };
        CelestiaComponent::CelestiaGamepadAction gamepadRemapLB{ CelestiaComponent::CelestiaGamepadAction::None };
        CelestiaComponent::CelestiaGamepadAction gamepadRemapRB{ CelestiaComponent::CelestiaGamepadAction::None };
        CelestiaComponent::CelestiaGamepadAction gamepadRemapDpadUp{ CelestiaComponent::CelestiaGamepadAction::PitchUp };
        CelestiaComponent::CelestiaGamepadAction gamepadRemapDpadDown{ CelestiaComponent::CelestiaGamepadAction::PitchDown };
        CelestiaComponent::CelestiaGamepadAction gamepadRemapDpadLeft{ CelestiaComponent::CelestiaGamepadAction::RollLeft };
        CelestiaComponent::CelestiaGamepadAction gamepadRemapDpadRight{ CelestiaComponent::CelestiaGamepadAction::RollRight };

        bool gamepadInvertX{ false };
        bool gamepadInvertY{ false };
        bool gamepadEnableLeftThumbstick{ true };
        bool gamepadEnableRightThumbstick{ true };
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct AppSettings : AppSettingsT<AppSettings, implementation::AppSettings>
    {
    };
}
