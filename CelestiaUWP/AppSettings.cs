//
// AppSettings.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaComponent;
using Windows.Storage;

namespace CelestiaUWP
{
    public class AppSettings
    {
        public static AppSettings Shared = new AppSettings();

        public bool UseFullDPI;
        public bool EnableMSAA;
        public string LanguageOverride;
        public string LastNewsID;
        public CelestiaGamepadAction GamepadRemapA;
        public CelestiaGamepadAction GamepadRemapB;
        public CelestiaGamepadAction GamepadRemapX;
        public CelestiaGamepadAction GamepadRemapY;
        public CelestiaGamepadAction GamepadRemapLT;
        public CelestiaGamepadAction GamepadRemapLB;
        public CelestiaGamepadAction GamepadRemapRT;
        public CelestiaGamepadAction GamepadRemapRB;
        public CelestiaGamepadAction GamepadRemapDpadLeft;
        public CelestiaGamepadAction GamepadRemapDpadRight;
        public CelestiaGamepadAction GamepadRemapDpadUp;
        public CelestiaGamepadAction GamepadRemapDpadDown;
        public bool GamepadInvertX;
        public bool GamepadInvertY;

        public static CelestiaGamepadAction GetAction(ApplicationDataContainer settings, string key, CelestiaGamepadAction defaultAction = CelestiaGamepadAction.None)
        {
            var value = settings.Values[key];
            if (value is int intValue)
                return (CelestiaGamepadAction)intValue;
            return defaultAction;
        }

        public static bool GetBool(ApplicationDataContainer settings, string key, bool defaultValue = false)
        {
            var value = settings.Values[key];
            if (value is bool boolean)
                return boolean;
            return defaultValue;
        }

        private AppSettings()
        {
            var settings = ApplicationData.Current.LocalSettings;
            UseFullDPI = GetBool(settings, "FullDPI", true);
            EnableMSAA = GetBool(settings, "EnableMSAA");

            var lang = settings.Values["LanguageOverride"];
            if (lang is string langValue)
                LanguageOverride = langValue;

            var lni = settings.Values["LastNewsID"];
            if (lni is string lniValue)
                LastNewsID = lniValue;

            GamepadRemapA = GetAction(settings, "GamepadRemapA", CelestiaGamepadAction.MoveSlower);
            GamepadRemapB = GetAction(settings, "GamepadRemapB");
            GamepadRemapX = GetAction(settings, "GamepadRemapX", CelestiaGamepadAction.MoveFaster);
            GamepadRemapY = GetAction(settings, "GamepadRemapY");
            GamepadRemapLT = GetAction(settings, "GamepadRemapLT", CelestiaGamepadAction.RollLeft);
            GamepadRemapLB = GetAction(settings, "GamepadRemapLB");
            GamepadRemapRT = GetAction(settings, "GamepadRemapRT", CelestiaGamepadAction.RollRight);
            GamepadRemapRB = GetAction(settings, "GamepadRemapRB");
            GamepadRemapDpadLeft = GetAction(settings, "GamepadRemapDpadLeft", CelestiaGamepadAction.RollLeft);
            GamepadRemapDpadRight = GetAction(settings, "GamepadRemapDpadRight", CelestiaGamepadAction.RollRight);
            GamepadRemapDpadUp = GetAction(settings, "GamepadRemapDpadUp", CelestiaGamepadAction.PitchUp);
            GamepadRemapDpadDown = GetAction(settings, "GamepadRemapDpadDown", CelestiaGamepadAction.PitchDown);
            GamepadInvertX = GetBool(settings, "GamepadInvertX");
            GamepadInvertY = GetBool(settings, "GamepadInvertY");
        }

        public void Save()
        {
            var settings = ApplicationData.Current.LocalSettings;
            settings.Values["FullDPI"] = UseFullDPI;
            settings.Values["EnableMSAA"] = EnableMSAA;
            settings.Values["LanguageOverride"] = LanguageOverride;
            settings.Values["LastNewsID"] = LastNewsID;
            settings.Values["GamepadRemapA"] = (int)GamepadRemapA;
            settings.Values["GamepadRemapB"] = (int)GamepadRemapB;
            settings.Values["GamepadRemapX"] = (int)GamepadRemapX;
            settings.Values["GamepadRemapY"] = (int)GamepadRemapY;
            settings.Values["GamepadRemapLT"] = (int)GamepadRemapLT;
            settings.Values["GamepadRemapLB"] = (int)GamepadRemapLB;
            settings.Values["GamepadRemapRT"] = (int)GamepadRemapRT;
            settings.Values["GamepadRemapRB"] = (int)GamepadRemapRB;
            settings.Values["GamepadRemapDpadLeft"] = (int)GamepadRemapDpadLeft;
            settings.Values["GamepadRemapDpadRight"] = (int)GamepadRemapDpadRight;
            settings.Values["GamepadRemapDpadUp"] = (int)GamepadRemapDpadUp;
            settings.Values["GamepadRemapDpadDown"] = (int)GamepadRemapDpadDown;
            settings.Values["GamepadInvertX"] = GamepadInvertX;
            settings.Values["GamepadInvertY"] = GamepadInvertY;
        }
    }
}
