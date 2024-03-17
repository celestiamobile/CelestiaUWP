//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "SettingCommonParameter.g.h"
#include "SettingTemplateSelector.g.h"
#include "SettingCommonPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct SettingCommonParameter : SettingCommonParameterT<SettingCommonParameter>
    {
        SettingCommonParameter(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& settingItems, bool showRestartHint);
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Items();
        bool ShowRestartHint();
    private:
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> settingItems;
        bool showRestartHint;
    };

    struct SettingTemplateSelector : SettingTemplateSelectorT<SettingTemplateSelector>
    {
        SettingTemplateSelector();
        Windows::UI::Xaml::DataTemplate Toggle();
        void Toggle(Windows::UI::Xaml::DataTemplate const&);
        Windows::UI::Xaml::DataTemplate Selection();
        void Selection(Windows::UI::Xaml::DataTemplate const&);
        Windows::UI::Xaml::DataTemplate Slider();
        void Slider(Windows::UI::Xaml::DataTemplate const&);
        Windows::UI::Xaml::DataTemplate Header();
        void Header(Windows::UI::Xaml::DataTemplate const&);

        Windows::UI::Xaml::DataTemplate SelectTemplateCore(Windows::Foundation::IInspectable const& item, Windows::UI::Xaml::DependencyObject const&);
        Windows::UI::Xaml::DataTemplate SelectTemplateCore(Windows::Foundation::IInspectable const& item);

    private:
        Windows::UI::Xaml::DataTemplate toggle{ nullptr };
        Windows::UI::Xaml::DataTemplate selection{ nullptr };
        Windows::UI::Xaml::DataTemplate slider{ nullptr };
        Windows::UI::Xaml::DataTemplate header{ nullptr };
    };

    struct SettingCommonPage : SettingCommonPageT<SettingCommonPage>
    {
        SettingCommonPage();
        void InitializeComponent();
    
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> Items();
        bool ShowRestartHint();

        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> items;
        bool showRestartHint;
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct SettingCommonParameter : SettingCommonParameterT<SettingCommonParameter, implementation::SettingCommonParameter>
    {
    };

    struct SettingTemplateSelector : SettingTemplateSelectorT<SettingTemplateSelector, implementation::SettingTemplateSelector>
    {
    };

    struct SettingCommonPage : SettingCommonPageT<SettingCommonPage, implementation::SettingCommonPage>
    {
    };
}
