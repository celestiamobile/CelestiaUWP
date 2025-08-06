// SettingCommonUserControl.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "SettingTemplateSelector.g.h"
#include "SettingCommonUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct SettingTemplateSelector : SettingTemplateSelectorT<SettingTemplateSelector>
    {
        SettingTemplateSelector();
        Microsoft::UI::Xaml::DataTemplate Toggle();
        void Toggle(Microsoft::UI::Xaml::DataTemplate const&);
        Microsoft::UI::Xaml::DataTemplate Selection();
        void Selection(Microsoft::UI::Xaml::DataTemplate const&);
        Microsoft::UI::Xaml::DataTemplate Slider();
        void Slider(Microsoft::UI::Xaml::DataTemplate const&);
        Microsoft::UI::Xaml::DataTemplate Header();
        void Header(Microsoft::UI::Xaml::DataTemplate const&);

        Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(Windows::Foundation::IInspectable const& item, Microsoft::UI::Xaml::DependencyObject const&);
        Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(Windows::Foundation::IInspectable const& item);

    private:
        Microsoft::UI::Xaml::DataTemplate toggle{ nullptr };
        Microsoft::UI::Xaml::DataTemplate selection{ nullptr };
        Microsoft::UI::Xaml::DataTemplate slider{ nullptr };
        Microsoft::UI::Xaml::DataTemplate header{ nullptr };
    };

    struct SettingCommonUserControl : SettingCommonUserControlT<SettingCommonUserControl>
    {
        SettingCommonUserControl(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& settingItems, bool showRestartHint);
        void InitializeComponent();

        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Items();
        bool ShowRestartHint();

    private:
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> items;
        bool showRestartHint;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct SettingTemplateSelector : SettingTemplateSelectorT<SettingTemplateSelector, implementation::SettingTemplateSelector>
    {
    };

    struct SettingCommonUserControl : SettingCommonUserControlT<SettingCommonUserControl, implementation::SettingCommonUserControl>
    {
    };
}
