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
        static hstring GetOverviewForSelection(CelestiaComponent::CelestiaSelection const& selection, CelestiaComponent::LocalizationProvider const& localizationProvider, CelestiaComponent::CelestiaAppCore const& appCore);
        static void InvokeCelestiaGamepadAction(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaGamepadAction action, bool up);
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaExtension : CelestiaExtensionT<CelestiaExtension, implementation::CelestiaExtension>
    {
    };
}
