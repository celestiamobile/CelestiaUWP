#include "pch.h"
#include "SettingItem.h"
#if __has_include("SettingHeaderItem.g.cpp")
#include "SettingHeaderItem.g.cpp"
#endif
#if __has_include("OptionPair.g.cpp")
#include "OptionPair.g.cpp"
#endif
#if __has_include("AppCoreBooleanItem.g.cpp")
#include "AppCoreBooleanItem.g.cpp"
#endif
#if __has_include("AppCoreInt32Item.g.cpp")
#include "AppCoreInt32Item.g.cpp"
#endif
#if __has_include("AppCoreSingleItem.g.cpp")
#include "AppCoreSingleItem.g.cpp"
#endif
#if __has_include("AppSettingsBooleanItem.g.cpp")
#include "AppSettingsBooleanItem.g.cpp"
#endif
#if __has_include("AppSettingsInt32Item.g.cpp")
#include "AppSettingsInt32Item.g.cpp"
#endif
#if __has_include("LanguageInt32Item.g.cpp")
#include "LanguageInt32Item.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Windows::Globalization;

namespace winrt::CelestiaAppComponent::implementation
{
    SettingHeaderItem::SettingHeaderItem(hstring const& title) : title(title) {};

    hstring SettingHeaderItem::Title()
    {
        return title;
    }

    OptionPair::OptionPair(int32_t value, hstring const& name) : value(value), name(name) {};

    int32_t OptionPair::Value()
    {
        return value;
    }

    hstring OptionPair::Name()
    {
        return name;
    }

    AppCoreBooleanItem::AppCoreBooleanItem(hstring const& title, CelestiaAppCore const& appCore, CelestiaSettingBooleanEntry entry, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note) : appCore(appCore), title(title), entry(entry), localSettings(localSettings), note(note)
    {
    }

    bool AppCoreBooleanItem::IsEnabled()
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            hasCorrectValue = true;
        return CelestiaExtension::GetCelestiaBooleanValue(appCore, entry);
    }

    void AppCoreBooleanItem::IsEnabled(bool value)
    {
        if (!hasCorrectValue)
            return;
        CelestiaExtension::SetCelestiaBooleanValue(appCore, entry, value);
        auto key = CelestiaExtension::GetNameByBooleanEntry(entry);
        if (!key.empty())
            localSettings.Values().Insert(key, box_value(value));
    }

    hstring AppCoreBooleanItem::Title()
    {
        return title;
    }

    hstring AppCoreBooleanItem::Note()
    {
        return note;
    }

    bool AppCoreBooleanItem::NoteVisibility()
    {
        return !note.empty();
    }

    AppCoreInt32Item::AppCoreInt32Item(hstring const& title, CelestiaAppCore const& appCore, CelestiaSettingInt32Entry entry, Collections::IVector<CelestiaAppComponent::OptionPair> const& options, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note) : appCore(appCore), title(title), entry(entry), options(options), localSettings(localSettings), note(note)
    {
        itemTitles = single_threaded_observable_vector<hstring>();
        for (auto const& option : options)
            itemTitles.Append(option.Name());
    }

    int32_t AppCoreInt32Item::Value()
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            hasCorrectValue = true;
        auto value = CelestiaExtension::GetCelestiaInt32Value(appCore, entry);
        for (auto const& option : options)
            if (option.Value() == value)
                return value;
        return 0;
    }

    void AppCoreInt32Item::Value(int32_t value)
    {
        if (!hasCorrectValue)
            return;
        auto actualValue = options.GetAt(value).Value();
        CelestiaExtension::SetCelestiaInt32Value(appCore, entry, actualValue);
        auto key = CelestiaExtension::GetNameByInt32Entry(entry);
        if (!key.empty())
            localSettings.Values().Insert(key, box_value(actualValue));
    }

    hstring AppCoreInt32Item::Title()
    {
        return title;
    }

    Collections::IObservableVector<hstring> AppCoreInt32Item::Options()
    {
        return itemTitles;
    }

    hstring AppCoreInt32Item::Note()
    {
        return note;
    }

    bool AppCoreInt32Item::NoteVisibility()
    {
        return !note.empty();
    }

    AppCoreSingleItem::AppCoreSingleItem(hstring const& title, CelestiaAppCore const& appCore, CelestiaSettingSingleEntry entry, float minValue, float maxValue, float step, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note) : appCore(appCore), title(title), entry(entry), minValue(minValue), maxValue(maxValue), step(step), localSettings(localSettings), note(note)
    {
    }

    double AppCoreSingleItem::Value()
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            hasCorrectValue = true;
        return (double)CelestiaExtension::GetCelestiaSingleValue(appCore, entry);
    }

    void AppCoreSingleItem::Value(double value)
    {
        if (!hasCorrectValue)
            return;
        CelestiaExtension::SetCelestiaSingleValue(appCore, entry, (float)value);
        auto key = CelestiaExtension::GetNameBySingleEntry(entry);
        if (!key.empty())
            localSettings.Values().Insert(key, box_value((float)value));
    }

    hstring AppCoreSingleItem::Title()
    {
        return title;
    }

    double AppCoreSingleItem::MinValue()
    {
        return (double)minValue;
    }

    double AppCoreSingleItem::MaxValue()
    {
        return (double)maxValue;
    }

    double AppCoreSingleItem::Step()
    {
        return (double)step;
    }

    hstring AppCoreSingleItem::Note()
    {
        return note;
    }

    bool AppCoreSingleItem::NoteVisibility()
    {
        return !note.empty();
    }

    AppSettingsBooleanItem::AppSettingsBooleanItem(hstring const& title, AppSettings const& appSettings, AppSettingBooleanEntry entry, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note) : title(title), appSettings(appSettings), entry(entry), localSettings(localSettings), note(note)
    {
    }

    bool AppSettingsBooleanItem::IsEnabled()
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            hasCorrectValue = true;
        switch (entry)
        {
        case CelestiaAppComponent::AppSettingBooleanEntry::UseFullDPI:
            return appSettings.UseFullDPI();
        case CelestiaAppComponent::AppSettingBooleanEntry::EnableMSAA:
            return appSettings.EnableMSAA();
        case CelestiaAppComponent::AppSettingBooleanEntry::GamepadInvertX:
            return appSettings.GamepadInvertX();
        case CelestiaAppComponent::AppSettingBooleanEntry::GamepadInvertY:
            return appSettings.GamepadInvertY();
        default:
            break;
        }
        return false;
    }

    void AppSettingsBooleanItem::IsEnabled(bool value)
    {
        if (!hasCorrectValue)
            return;
        switch (entry)
        {
        case CelestiaAppComponent::AppSettingBooleanEntry::UseFullDPI:
            appSettings.UseFullDPI(value);
            break;
        case CelestiaAppComponent::AppSettingBooleanEntry::EnableMSAA:
            appSettings.EnableMSAA(value);
            break;
        case CelestiaAppComponent::AppSettingBooleanEntry::GamepadInvertX:
            appSettings.GamepadInvertX(value);
            break;
        case CelestiaAppComponent::AppSettingBooleanEntry::GamepadInvertY:
            appSettings.GamepadInvertY(value);
            break;
        default:
            return;
        }
        appSettings.Save(localSettings);
    }

    hstring AppSettingsBooleanItem::Title()
    {
        return title;
    }

    hstring AppSettingsBooleanItem::Note()
    {
        return note;
    }

    bool AppSettingsBooleanItem::NoteVisibility()
    {
        return !note.empty();
    }

    AppSettingsInt32Item::AppSettingsInt32Item(hstring const& title, AppSettings const& appSettings, AppSettingInt32Entry entry, Collections::IVector<CelestiaAppComponent::OptionPair> const& options, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note) : appSettings(appSettings), title(title), entry(entry), options(options), localSettings(localSettings), note(note)
    {
        itemTitles = single_threaded_observable_vector<hstring>();
        for (auto&& option : options)
            itemTitles.Append(option.Name());
    }

    int32_t AppSettingsInt32Item::Value()
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            hasCorrectValue = true;

        int32_t value = 0;
        switch (entry)
        {
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapA:
            value = static_cast<int32_t>(appSettings.GamepadRemapA());
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapB:
            value = static_cast<int32_t>(appSettings.GamepadRemapB());
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapX:
            value = static_cast<int32_t>(appSettings.GamepadRemapX());
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapY:
            value = static_cast<int32_t>(appSettings.GamepadRemapY());
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapLT:
            value = static_cast<int32_t>(appSettings.GamepadRemapLT());
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapRT:
            value = static_cast<int32_t>(appSettings.GamepadRemapRT());
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapLB:
            value = static_cast<int32_t>(appSettings.GamepadRemapLB());
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapRB:
            value = static_cast<int32_t>(appSettings.GamepadRemapRB());
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapDpadUp:
            value = static_cast<int32_t>(appSettings.GamepadRemapDpadUp());
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapDpadDown:
            value = static_cast<int32_t>(appSettings.GamepadRemapDpadDown());
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapDpadLeft:
            value = static_cast<int32_t>(appSettings.GamepadRemapDpadLeft());
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapDpadRight:
            value = static_cast<int32_t>(appSettings.GamepadRemapDpadRight());
            break;
        default:
            break;
        }
        for (uint32_t i = 0; i < options.Size(); i += 1)
            if (options.GetAt(i).Value() == value)
                return i;
        return 0;
    }

    void AppSettingsInt32Item::Value(int32_t value)
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            return;

        auto actualValue = options.GetAt(value).Value();
        switch (entry)
        {
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapA:
            appSettings.GamepadRemapA(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapB:
            appSettings.GamepadRemapB(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapX:
            appSettings.GamepadRemapX(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapY:
            appSettings.GamepadRemapY(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapLT:
            appSettings.GamepadRemapLT(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapRT:
            appSettings.GamepadRemapRT(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapLB:
            appSettings.GamepadRemapLB(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapRB:
            appSettings.GamepadRemapRB(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapDpadUp:
            appSettings.GamepadRemapDpadUp(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapDpadDown:
            appSettings.GamepadRemapDpadDown(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapDpadLeft:
            appSettings.GamepadRemapDpadLeft(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        case CelestiaAppComponent::AppSettingInt32Entry::GamepadRemapDpadRight:
            appSettings.GamepadRemapDpadRight(static_cast<CelestiaGamepadAction>(actualValue));
            break;
        default:
            return;
        }
        appSettings.Save(localSettings);
    }

    hstring AppSettingsInt32Item::Title()
    {
        return title;
    }

    Collections::IObservableVector<hstring> AppSettingsInt32Item::Options()
    {
        return itemTitles;
    }

    hstring AppSettingsInt32Item::Note()
    {
        return note;
    }

    bool AppSettingsInt32Item::NoteVisibility()
    {
        return !note.empty();
    }

    LanguageInt32Item::LanguageInt32Item(hstring const& title, AppSettings const& appSettings, Collections::IVector<hstring> const& availableLanguages, Windows::Storage::ApplicationDataContainer const& localSettings) : title(title), appSettings(appSettings), localSettings(localSettings), availableLanguages(availableLanguages)
    {
        itemTitles = single_threaded_observable_vector<hstring>();
        itemTitles.Append(L"System");
        for (const auto& language : availableLanguages)
        {
            auto lang = language;
            if (lang == L"zh_CN")
            {
                lang = L"zh-Hans";
            }
            else if (lang == L"zh_TW")
            {
                lang = L"zh-Hant";
            }
            else
            {
                lang = LocalizationHelper::ToWindowsTag(lang);
            }
            auto culture = Language(lang);
            itemTitles.Append(culture.NativeName());
        }
    }

    int32_t LanguageInt32Item::Value()
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            hasCorrectValue = true;
        auto selectedLang = ApplicationLanguages::PrimaryLanguageOverride();
        if (selectedLang.empty()) return 0;
        uint32_t index;
        if (availableLanguages.IndexOf(LocalizationHelper::FromWindowsTag(selectedLang), index))
            return index + 1;
        return 0;
    }

    void LanguageInt32Item::Value(int32_t value)
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            return;
        if (value == 0)
            ApplicationLanguages::PrimaryLanguageOverride(L"");
        else
            ApplicationLanguages::PrimaryLanguageOverride(LocalizationHelper::ToWindowsTag(availableLanguages.GetAt(value - 1)));
    }

    hstring LanguageInt32Item::Title()
    {
        return title;
    }

    Collections::IObservableVector<hstring> LanguageInt32Item::Options()
    {
        return itemTitles;
    }

    hstring LanguageInt32Item::Note()
    {
        return L"";
    }

    bool LanguageInt32Item::NoteVisibility()
    {
        return false;
    }
}
