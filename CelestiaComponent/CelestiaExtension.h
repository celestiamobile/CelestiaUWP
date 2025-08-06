// CelestiaExtension.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "CelestiaExtension.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaExtension : CelestiaExtensionT<CelestiaExtension>
    {
        static com_array<CelestiaComponent::CelestiaBrowserItem> GetChildren(CelestiaComponent::CelestiaBrowserItem const& item, CelestiaComponent::CelestiaAppCore const& appCore);
        static CelestiaComponent::CelestiaSettingBooleanEntry GetBooleanEntryByName(hstring const& name);
        static CelestiaComponent::CelestiaSettingInt32Entry GetInt32EntryByName(hstring const& name);
        static CelestiaComponent::CelestiaSettingSingleEntry GetSingleEntryByName(hstring const& name);
        static hstring GetNameByBooleanEntry(CelestiaComponent::CelestiaSettingBooleanEntry entry);
        static hstring GetNameByInt32Entry(CelestiaComponent::CelestiaSettingInt32Entry entry);
        static hstring GetNameBySingleEntry(CelestiaComponent::CelestiaSettingSingleEntry entry);
        static void SetCelestiaBooleanValue(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingBooleanEntry entry, bool value);
        static void SetCelestiaInt32Value(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingInt32Entry entry, int32_t value);
        static void SetCelestiaSingleValue(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingSingleEntry entry, float value);
        static bool GetCelestiaBooleanValue(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingBooleanEntry entry);
        static int32_t GetCelestiaInt32Value(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingInt32Entry entry);
        static float GetCelestiaSingleValue(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingSingleEntry entry);
        static void InvokeCelestiaGamepadAction(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaGamepadAction action, bool up);
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaExtension : CelestiaExtensionT<CelestiaExtension, implementation::CelestiaExtension>
    {
    };
}
