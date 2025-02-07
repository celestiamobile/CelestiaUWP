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
#if __has_include("AppSettingsDoubleItem.g.cpp")
#include "AppSettingsDoubleItem.g.cpp"
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
    CRITICAL_SECTION appCoreCritSection;
    bool appCoreCritSectionInitialized = false;

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

    AppCoreBooleanItem::AppCoreBooleanItem(hstring const& title, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, CelestiaSettingBooleanEntry entry, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note) : appCore(appCore), renderer(renderer), title(title), entry(entry), localSettings(localSettings), note(note)
    {
        if (!appCoreCritSectionInitialized)
        {
            InitializeCriticalSection(&appCoreCritSection);
            appCoreCritSectionInitialized = true;
        }
    }

    bool AppCoreBooleanItem::IsEnabled()
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            hasCorrectValue = true;
        std::optional<bool> savedValue;
        EnterCriticalSection(&appCoreCritSection);
        savedValue = cachedValue;
        LeaveCriticalSection(&appCoreCritSection);
        return savedValue.value_or(CelestiaExtension::GetCelestiaBooleanValue(appCore, entry));
    }

    void AppCoreBooleanItem::IsEnabled(bool value)
    {
        if (!hasCorrectValue)
            return;
        EnterCriticalSection(&appCoreCritSection);
        cachedValue = value;
        LeaveCriticalSection(&appCoreCritSection);
        renderer.EnqueueTask([weak_this{ get_weak() }, value]
            {
                auto strong_this = weak_this.get();
                if (!strong_this)
                    return;

                CelestiaExtension::SetCelestiaBooleanValue(strong_this->appCore, strong_this->entry, value);
                EnterCriticalSection(&appCoreCritSection);
                strong_this->cachedValue = std::nullopt;
                LeaveCriticalSection(&appCoreCritSection);
            });
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

    AppCoreInt32Item::AppCoreInt32Item(hstring const& title, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, CelestiaSettingInt32Entry entry, Collections::IVector<CelestiaAppComponent::OptionPair> const& options, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note) : appCore(appCore), renderer(renderer), title(title), entry(entry), options(options), localSettings(localSettings), note(note)
    {
        if (!appCoreCritSectionInitialized)
        {
            InitializeCriticalSection(&appCoreCritSection);
            appCoreCritSectionInitialized = true;
        }
        itemTitles = single_threaded_observable_vector<hstring>();
        for (auto const& option : options)
            itemTitles.Append(option.Name());
    }

    int32_t AppCoreInt32Item::Value()
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            hasCorrectValue = true;
        std::optional<int32_t> savedValue;
        EnterCriticalSection(&appCoreCritSection);
        savedValue = cachedValue;
        LeaveCriticalSection(&appCoreCritSection);
        auto value = savedValue.value_or(CelestiaExtension::GetCelestiaInt32Value(appCore, entry));
        for (uint32_t i = 0; i < options.Size(); i += 1)
            if (options.GetAt(i).Value() == value)
                return i;
        return 0;
    }

    void AppCoreInt32Item::Value(int32_t value)
    {
        if (!hasCorrectValue)
            return;
        auto actualValue = options.GetAt(value).Value();
        EnterCriticalSection(&appCoreCritSection);
        cachedValue = actualValue;
        LeaveCriticalSection(&appCoreCritSection);
        renderer.EnqueueTask([weak_this{ get_weak() }, actualValue]
            {
                auto strong_this = weak_this.get();
                if (!strong_this)
                    return;

                CelestiaExtension::SetCelestiaInt32Value(strong_this->appCore, strong_this->entry, actualValue);
                EnterCriticalSection(&appCoreCritSection);
                strong_this->cachedValue = std::nullopt;
                LeaveCriticalSection(&appCoreCritSection);
            });
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

    AppCoreSingleItem::AppCoreSingleItem(hstring const& title, CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, CelestiaSettingSingleEntry entry, float minValue, float maxValue, float step, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note) : appCore(appCore), renderer(renderer), title(title), entry(entry), minValue(minValue), maxValue(maxValue), step(step), localSettings(localSettings), note(note)
    {
        if (!appCoreCritSectionInitialized)
        {
            InitializeCriticalSection(&appCoreCritSection);
            appCoreCritSectionInitialized = true;
        }
    }

    double AppCoreSingleItem::Value()
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            hasCorrectValue = true;
        std::optional<float> savedValue;
        EnterCriticalSection(&appCoreCritSection);
        savedValue = cachedValue;
        LeaveCriticalSection(&appCoreCritSection);
        return static_cast<double>(savedValue.value_or(CelestiaExtension::GetCelestiaSingleValue(appCore, entry)));
    }

    void AppCoreSingleItem::Value(double value)
    {
        if (!hasCorrectValue)
            return;
        EnterCriticalSection(&appCoreCritSection);
        cachedValue = static_cast<float>(value);
        LeaveCriticalSection(&appCoreCritSection);
        renderer.EnqueueTask([weak_this{ get_weak() }, value]
            {
                auto strong_this = weak_this.get();
                if (!strong_this)
                    return;

                CelestiaExtension::SetCelestiaSingleValue(strong_this->appCore, strong_this->entry, static_cast<float>(value));
                EnterCriticalSection(&appCoreCritSection);
                strong_this->cachedValue = std::nullopt;
                LeaveCriticalSection(&appCoreCritSection);
            });
        auto key = CelestiaExtension::GetNameBySingleEntry(entry);
        if (!key.empty())
            localSettings.Values().Insert(key, box_value(static_cast<float>(value)));
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
        return appSettings.GetBoolean(entry);
    }

    void AppSettingsBooleanItem::IsEnabled(bool value)
    {
        if (!hasCorrectValue)
            return;
        appSettings.SetBoolean(entry, value);
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

        int32_t value = appSettings.GetInt32(entry);
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
        appSettings.SetInt32(entry, actualValue);
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

    AppSettingsDoubleItem::AppSettingsDoubleItem(hstring const& title, AppSettings const& appSettings, AppSettingDoubleEntry entry, double minValue, double maxValue, double step, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note) : appSettings(appSettings), title(title), entry(entry), minValue(minValue), maxValue(maxValue), step(step), localSettings(localSettings), note(note)
    {
    }

    double AppSettingsDoubleItem::Value()
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            hasCorrectValue = true;

        return appSettings.GetDouble(entry);
    }

    void AppSettingsDoubleItem::Value(double value)
    {
        // Must be queried before setting
        if (!hasCorrectValue)
            return;

        appSettings.SetDouble(entry, value);
        appSettings.Save(localSettings);
    }

    double AppSettingsDoubleItem::MinValue()
    {
        return minValue;
    }

    double AppSettingsDoubleItem::MaxValue()
    {
        return maxValue;
    }

    double AppSettingsDoubleItem::Step()
    {
        return step;
    }

    hstring AppSettingsDoubleItem::Title()
    {
        return title;
    }

    hstring AppSettingsDoubleItem::Note()
    {
        return note;
    }

    bool AppSettingsDoubleItem::NoteVisibility()
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
        auto selectedLang = Microsoft::Windows::Globalization::ApplicationLanguages::PrimaryLanguageOverride();
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
            Microsoft::Windows::Globalization::ApplicationLanguages::PrimaryLanguageOverride(L"");
        else
            Microsoft::Windows::Globalization::ApplicationLanguages::PrimaryLanguageOverride(LocalizationHelper::ToWindowsTag(availableLanguages.GetAt(value - 1)));
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
