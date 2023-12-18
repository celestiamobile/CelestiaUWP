// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "SettingBaseItem.g.h"
#include "SettingBooleanItem.g.h"
#include "SettingInt32Item.g.h"
#include "SettingDoubleItem.g.h"
#include "SettingHeaderItem.g.h"
#include "OptionPair.g.h"
#include "AppCoreBooleanItem.g.h"
#include "AppCoreInt32Item.g.h"
#include "AppCoreSingleItem.g.h"
#include "AppSettingsBooleanItem.g.h"
#include "AppSettingsInt32Item.g.h"
#include "AppSettingsDoubleItem.g.h"
#include "LanguageInt32Item.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct SettingBaseItem : SettingBaseItemT<SettingBaseItem>
    {
    };

    struct SettingBooleanItem : SettingBooleanItemT<SettingBooleanItem, SettingBaseItem>
    {
        virtual bool IsEnabled() = 0;
        virtual void IsEnabled(bool) = 0;
        virtual hstring Title() = 0;
        virtual hstring Note() = 0;
        virtual bool NoteVisibility() = 0;

    protected:
        bool hasCorrectValue{ false };
    };

    struct SettingInt32Item : SettingInt32ItemT<SettingInt32Item, SettingBaseItem>
    {
        virtual int32_t Value() = 0;
        virtual void Value(int32_t) = 0;
        virtual hstring Title() = 0;
        virtual Windows::Foundation::Collections::IObservableVector<hstring> Options() = 0;
        virtual hstring Note() = 0;
        virtual bool NoteVisibility() = 0;

    protected:
        bool hasCorrectValue{ false };
    };

    struct SettingDoubleItem : SettingDoubleItemT<SettingDoubleItem, SettingBaseItem>
    {
        virtual double Value() = 0;
        virtual void Value(double) = 0;
        virtual hstring Title() = 0;
        virtual double MinValue() = 0;
        virtual double MaxValue() = 0;
        virtual double Step() = 0;
        virtual hstring Note() = 0;
        virtual bool NoteVisibility() = 0;

    protected:
        bool hasCorrectValue{ false };
    };

    struct SettingHeaderItem : SettingHeaderItemT<SettingHeaderItem, SettingBaseItem>
    {
        SettingHeaderItem(hstring const& title);
        hstring Title();

    private:
        hstring title;
    };

    struct OptionPair : OptionPairT<OptionPair>
    {
        OptionPair(int32_t value, hstring const& name);
        int32_t Value();
        hstring Name();
    private:
        int32_t value;
        hstring name;
    };;

    struct AppCoreBooleanItem : AppCoreBooleanItemT<AppCoreBooleanItem, SettingBooleanItem>
    {
        AppCoreBooleanItem(hstring const& title, CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const &renderer, CelestiaComponent::CelestiaSettingBooleanEntry entry, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note = L"");
        bool IsEnabled();
        void IsEnabled(bool);
        hstring Title();
        hstring Note();
        bool NoteVisibility();

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        hstring title;
        CelestiaComponent::CelestiaSettingBooleanEntry entry;
        Windows::Storage::ApplicationDataContainer localSettings;
        hstring note;
        std::optional<bool> cachedValue{ std::nullopt };
    };

    struct AppCoreInt32Item : AppCoreInt32ItemT<AppCoreInt32Item, SettingInt32Item>
    {
        AppCoreInt32Item(hstring const& title, CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const &renderer, CelestiaComponent::CelestiaSettingInt32Entry entry, Windows::Foundation::Collections::IVector<CelestiaAppComponent::OptionPair> const& options, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note = L"");
        int32_t Value();
        void Value(int32_t);
        hstring Title();
        Windows::Foundation::Collections::IObservableVector<hstring> Options();
        hstring Note();
        bool NoteVisibility();

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        hstring title;
        CelestiaComponent::CelestiaSettingInt32Entry entry;
        Windows::Foundation::Collections::IVector<CelestiaAppComponent::OptionPair> options;
        Windows::Foundation::Collections::IObservableVector<hstring> itemTitles;
        Windows::Storage::ApplicationDataContainer localSettings;
        hstring note;
        std::optional<int32_t> cachedValue{ std::nullopt };
    };

    struct AppCoreSingleItem : AppCoreSingleItemT<AppCoreSingleItem, SettingDoubleItem>
    {
        AppCoreSingleItem(hstring const& title, CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const &renderer, CelestiaComponent::CelestiaSettingSingleEntry entry, float minValue, float maxValue, float step, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note = L"");
        double Value();
        void Value(double);
        hstring Title();
        double MinValue();
        double MaxValue();
        double Step();
        hstring Note();
        bool NoteVisibility();

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        hstring title;
        CelestiaComponent::CelestiaSettingSingleEntry entry;
        float minValue;
        float maxValue;
        float step;
        Windows::Storage::ApplicationDataContainer localSettings;
        hstring note;
        std::optional<float> cachedValue{ std::nullopt };
    };

    struct AppSettingsBooleanItem : AppSettingsBooleanItemT<AppSettingsBooleanItem, SettingBooleanItem>
    {
        AppSettingsBooleanItem(hstring const& title, CelestiaAppComponent::AppSettings const& appSettings, CelestiaAppComponent::AppSettingBooleanEntry entry, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note = L"");
        bool IsEnabled();
        void IsEnabled(bool);
        hstring Title();
        hstring Note();
        bool NoteVisibility();

    private:
        hstring title;
        CelestiaAppComponent::AppSettings appSettings;
        CelestiaAppComponent::AppSettingBooleanEntry entry;
        Windows::Storage::ApplicationDataContainer localSettings;
        hstring note;
    };

    struct AppSettingsInt32Item : AppSettingsInt32ItemT<AppSettingsInt32Item, SettingInt32Item>
    {
        AppSettingsInt32Item(hstring const& title, CelestiaAppComponent::AppSettings const& appSettings, CelestiaAppComponent::AppSettingInt32Entry entry, Windows::Foundation::Collections::IVector<CelestiaAppComponent::OptionPair> const& options, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note = L"");
        int32_t Value();
        void Value(int32_t);
        hstring Title();
        Windows::Foundation::Collections::IObservableVector<hstring> Options();
        hstring Note();
        bool NoteVisibility();

    private:
        CelestiaAppComponent::AppSettings appSettings;
        hstring title;
        CelestiaAppComponent::AppSettingInt32Entry entry;
        Windows::Foundation::Collections::IVector<CelestiaAppComponent::OptionPair> options;
        Windows::Foundation::Collections::IObservableVector<hstring> itemTitles;
        Windows::Storage::ApplicationDataContainer localSettings;
        hstring note;
    };

    struct AppSettingsDoubleItem : AppSettingsDoubleItemT<AppSettingsDoubleItem, SettingDoubleItem>
    {
        AppSettingsDoubleItem(hstring const& title, CelestiaAppComponent::AppSettings const& appSettings, CelestiaAppComponent::AppSettingDoubleEntry entry, double minValue, double maxValue, double step, Windows::Storage::ApplicationDataContainer const& localSettings, hstring const& note = L"");
        double Value();
        void Value(double);
        double MinValue();
        double MaxValue();
        double Step();
        hstring Title();
        hstring Note();
        bool NoteVisibility();

    private:
        CelestiaAppComponent::AppSettings appSettings;
        hstring title;
        double minValue;
        double maxValue;
        double step;
        CelestiaAppComponent::AppSettingDoubleEntry entry;
        Windows::Storage::ApplicationDataContainer localSettings;
        hstring note;
    };

    struct LanguageInt32Item : LanguageInt32ItemT<LanguageInt32Item, SettingInt32Item>
    {
        LanguageInt32Item(hstring const& title, CelestiaAppComponent::AppSettings const& appSettings, Windows::Foundation::Collections::IVector<hstring> const& availableLanguages, Windows::Storage::ApplicationDataContainer const& localSettings);
        int32_t Value();
        void Value(int32_t);
        hstring Title();
        Windows::Foundation::Collections::IObservableVector<hstring> Options();
        hstring Note();
        bool NoteVisibility();

    private:
        hstring title;
        CelestiaAppComponent::AppSettings appSettings;
        Windows::Storage::ApplicationDataContainer localSettings;
        Windows::Foundation::Collections::IVector<hstring> availableLanguages;
        Windows::Foundation::Collections::IObservableVector<hstring> itemTitles;
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct SettingHeaderItem : SettingHeaderItemT<SettingHeaderItem, implementation::SettingHeaderItem>
    {
    };

    struct OptionPair : OptionPairT<OptionPair, implementation::OptionPair>
    {
    };

    struct AppCoreBooleanItem : AppCoreBooleanItemT<AppCoreBooleanItem, implementation::AppCoreBooleanItem>
    {
    };

    struct AppCoreInt32Item : AppCoreInt32ItemT<AppCoreInt32Item, implementation::AppCoreInt32Item>
    {
    };

    struct AppCoreSingleItem : AppCoreSingleItemT<AppCoreSingleItem, implementation::AppCoreSingleItem>
    {
    };

    struct AppSettingsBooleanItem : AppSettingsBooleanItemT<AppSettingsBooleanItem, implementation::AppSettingsBooleanItem>
    {
    };

    struct AppSettingsInt32Item : AppSettingsInt32ItemT<AppSettingsInt32Item, implementation::AppSettingsInt32Item>
    {
    };

    struct AppSettingsDoubleItem : AppSettingsDoubleItemT<AppSettingsDoubleItem, implementation::AppSettingsDoubleItem>
    {
    };

    struct LanguageInt32Item : LanguageInt32ItemT<LanguageInt32Item, implementation::LanguageInt32Item>
    {
    };
}
