//
// SettingsCommonPage.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaComponent;
using System;
using System.Collections.Generic;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP.Settings
{
    public class SettingsCommonItem
    {
        public string Title;
    }

    public abstract class SettingsBooleanItem : SettingsCommonItem
    {
        public abstract bool IsEnabled { get; set; }
        public abstract string Note { get; }
        public abstract Visibility NoteVisibility { get; }
        protected bool hasCorrectValue = false;
    }

    public class AppCoreBooleanItem : SettingsBooleanItem
    {
        private CelestiaAppCore appCore;
        private CelestiaSettingBooleanEntry entry;
        private Windows.Storage.ApplicationDataContainer localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
        private string note;

        public AppCoreBooleanItem(string title, CelestiaAppCore appCore, CelestiaSettingBooleanEntry entry, string note = "")
        {
            this.Title = title;
            this.entry = entry;
            this.appCore = appCore;
            this.note = note;
        }

        public override bool IsEnabled
        {
            get
            {
                if (!hasCorrectValue)
                    hasCorrectValue = true;
                return CelestiaExtension.GetCelestiaBooleanValue(appCore, entry);
            }
            set
            {
                if (!hasCorrectValue) return;
                CelestiaExtension.SetCelestiaBooleanValue(appCore, entry, value);
                var key = CelestiaExtension.GetNameByBooleanEntry(entry);
                if (key != null && key.Length > 0)
                    localSettings.Values[key] = value;
            }
        }

        public override string Note => note;

        public override Visibility NoteVisibility => string.IsNullOrEmpty(note) ? Visibility.Collapsed : Visibility.Visible;
    }

    public class AppSettingsBooleanItem : SettingsBooleanItem
    {
        private AppSettings appSettings;
        private string key;
        private string note;

        public AppSettingsBooleanItem(string title, AppSettings appSettings, string key, string note = "")
        {
            this.Title = title;
            this.key = key;
            this.appSettings = appSettings;
            this.note = note;
        }

        public override bool IsEnabled
        {
            get
            {
                if (!hasCorrectValue)
                    hasCorrectValue = true;
                return (bool)appSettings.GetType().GetField(key).GetValue(appSettings);
            }
            set
            {
                if (!hasCorrectValue) return;
                appSettings.GetType().GetField(key).SetValue(appSettings, value);
                appSettings.Save();
            }
        }

        public override string Note => note;

        public override Visibility NoteVisibility => string.IsNullOrEmpty(note) ? Visibility.Collapsed : Visibility.Visible;
    }

    public abstract class SettingsIntItem : SettingsCommonItem
    {
        public abstract int Value { get; set; }
        public abstract string[] Options { get; }
        public abstract string Note { get; }
        public abstract Visibility NoteVisibility { get; }
        protected bool hasCorrectValue = false;
    }

    public class AppCoreIntItem : SettingsIntItem
    {
        private CelestiaAppCore appCore;
        private CelestiaSettingInt32Entry entry;
        private (string, int)[] options;
        private string[] itemTitles;
        private Windows.Storage.ApplicationDataContainer localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
        private string note;

        public AppCoreIntItem(string title, CelestiaAppCore appCore, CelestiaSettingInt32Entry entry, string[] itemTitles, string note = "")
        {
            Title = title;
            this.entry = entry;
            this.appCore = appCore;
            options = new (string, int)[itemTitles.Length];
            for (int i = 0; i < itemTitles.Length; i++)
                options[i] = (itemTitles[i], i);
            this.itemTitles = itemTitles;
            this.note = note;
        }
        public AppCoreIntItem(string title, CelestiaAppCore appCore, CelestiaSettingInt32Entry entry, (string, int)[] options, string note = "")
        {
            Title = title;
            this.entry = entry;
            this.appCore = appCore;
            this.options = options;
            itemTitles = new string[options.Length];
            for (int i = 0; i < options.Length; i++)
                itemTitles[i] = options[i].Item1;
            this.note = note;
        }

        public override int Value
        {
            get
            {
                if (!hasCorrectValue)
                    hasCorrectValue = true;
                var value = CelestiaExtension.GetCelestiaInt32Value(appCore, entry);
                for (var i = 0; i < options.Length; i += 1)
                {
                    if (options[i].Item2 == value)
                        return i;
                }
                return 0;
            }
            set
            {
                if (!hasCorrectValue) return;
                CelestiaExtension.SetCelestiaInt32Value(appCore, entry, options[value].Item2);
                var key = CelestiaExtension.GetNameByInt32Entry(entry);
                if (key != null && key.Length > 0)
                    localSettings.Values[key] = value;
            }
        }
        public override string[] Options => itemTitles;
        public override string Note => note;
        public override Visibility NoteVisibility => string.IsNullOrEmpty(note) ? Visibility.Collapsed : Visibility.Visible;
    }

    public class AppSettingsIntItem : SettingsIntItem
    {
        private AppSettings appSettings;
        private string key;
        private (string, int)[] options;
        private string[] itemTitles;
        private string note;
        public AppSettingsIntItem(string title, AppSettings appSettings, string key, string[] itemTitles, string note = "")
        {
            this.Title = title;
            this.key = key;
            this.appSettings = appSettings;
            this.itemTitles = itemTitles;
            options = new (string, int)[itemTitles.Length];
            for (int i = 0; i < itemTitles.Length; i++)
                options[i] = (itemTitles[i], i);
            this.note = note;
        }

        public AppSettingsIntItem(string title, AppSettings appSettings, string key, (string, int)[] options, string note = "")
        {
            this.Title = title;
            this.key = key;
            this.appSettings = appSettings;
            this.options = options;
            itemTitles = new string[options.Length];  
            for (int i = 0; i < options.Length; i++)
                itemTitles[i] = options[i].Item1;
            this.note = note;
        }

        public override int Value
        {
            get
            {
                if (!hasCorrectValue)
                    hasCorrectValue = true;
                var value = (int)appSettings.GetType().GetField(key).GetValue(appSettings);
                for (var i = 0; i < options.Length; i += 1)
                {
                    if (options[i].Item2 == value)
                        return i;
                }
                return 0;
            }
            set
            {
                if (!hasCorrectValue) return;
                appSettings.GetType().GetField(key).SetValue(appSettings, options[value].Item2);
                appSettings.Save();
            }
        }
        public override string[] Options => itemTitles;
        public override string Note => note;
        public override Visibility NoteVisibility => string.IsNullOrEmpty(note) ? Visibility.Collapsed : Visibility.Visible;

    }

    public class LanguageIntItem : SettingsIntItem
    {
        private AppSettings appSettings;
        private string[] itemTitles;
        private string[] availableLanguages;
        public LanguageIntItem(string title, AppSettings appSettings, string[] availableLanguages)
        {
            this.Title = title;
            this.appSettings = appSettings;
            this.availableLanguages = availableLanguages;

            var items = new List<string>();
            items.Add("System");
            foreach (var language in availableLanguages)
            {
                var lang = language;
                if (lang == "zh_CN")
                    lang = "zh-Hans";
                else if (lang == "zh_TW")
                    lang = "zh-Hant";
                else
                    lang = lang.Replace("_", "-");
                var culture = new System.Globalization.CultureInfo(lang);
                items.Add(culture.NativeName);
            }
            itemTitles = items.ToArray();
        }

        public override int Value
        {
            get
            {
                if (!hasCorrectValue)
                    hasCorrectValue = true;
                var selectedLang = appSettings.LanguageOverride;
                var selectedIndex = Array.IndexOf(availableLanguages, selectedLang);
                return selectedIndex + 1;
            }
            set
            {
                if (!hasCorrectValue) return;
                if (value == 0)
                {
                    appSettings.LanguageOverride = null;
                }
                else
                {
                    appSettings.LanguageOverride = availableLanguages[value - 1];
                }
                appSettings.Save();
            }
        }
        public override string[] Options => itemTitles;

        public override string Note => "";
        public override Visibility NoteVisibility => Visibility.Collapsed;
    }

    public class SettingsHeaderItem : SettingsCommonItem
    {
        public SettingsHeaderItem(string title)
        {
            this.Title = title;
        }
    }
    public abstract class SettingsDoubleItem : SettingsCommonItem
    {
        public abstract double Value { get; set; }
        public abstract double MinValue { get; }
        public abstract double MaxValue { get; }
        public abstract double Step { get; }
        public abstract string Note { get; }
        public abstract Visibility NoteVisibility { get; }
        protected bool hasCorrectValue = false;
    }

    public class AppCoreFloatItem : SettingsDoubleItem
    {
        private CelestiaAppCore appCore;
        private CelestiaSettingSingleEntry entry;
        private float minValue;
        private float maxValue;
        private float stepValue;
        private Windows.Storage.ApplicationDataContainer localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
        private string note;

        public AppCoreFloatItem(string title, CelestiaAppCore appCore, CelestiaSettingSingleEntry entry, float minValue, float maxValue, float stepValue = 1, string note = "")
        {
            this.Title = title;
            this.entry = entry;
            this.appCore = appCore;
            this.minValue = minValue;
            this.maxValue = maxValue;
            this.stepValue = stepValue;
            this.note = note;
        }

        public override double Value
        {
            get
            {
                if (!hasCorrectValue)
                    hasCorrectValue = true;
                return (float)CelestiaExtension.GetCelestiaSingleValue(appCore, entry);
            }
            set
            {
                if (!hasCorrectValue) return;
                CelestiaExtension.SetCelestiaSingleValue(appCore, entry, (float)value);
                var key = CelestiaExtension.GetNameBySingleEntry(entry);
                if (key != null && key.Length > 0)
                    localSettings.Values[key] = (float)value;
            }
        }

        public override double MinValue => minValue;
        public override double MaxValue => maxValue;
        public override double Step => stepValue;
        public override string Note => "";
        public override Visibility NoteVisibility => Visibility.Collapsed;
    }

    public class SettingsItemTemplateSelector : DataTemplateSelector
    {
        public DataTemplate Toggle { get; set; }
        public DataTemplate Slider { get; set; }
        public DataTemplate Selection { get; set; }
        public DataTemplate Header { get; set; }

        protected override DataTemplate SelectTemplateCore(object item)
        {
            if (item is SettingsBooleanItem)
            {
                return Toggle;
            }
            else if (item is SettingsDoubleItem)
            {
                return Slider;
            }
            else if (item is SettingsIntItem)
            {
                return Selection;
            }
            else if (item is SettingsHeaderItem)
            {
                return Header;
            }
            else
            {
                return null;
            }
        }
    }
    public sealed partial class SettingsCommonPage : Page
    {
        private SettingsCommonItem[] Items;

        public SettingsCommonPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            Items = (SettingsCommonItem[])e.Parameter;
        }
    }
}
