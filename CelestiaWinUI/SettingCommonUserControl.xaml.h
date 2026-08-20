// SettingCommonUserControl.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "SettingParameter.g.h"
#include "SettingTemplateSelector.g.h"
#include "SettingListItem.g.h"
#include "SettingGroupSeparator.g.h"
#include "SettingInfoItem.g.h"
#include "SettingCommonUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct SettingParameter : SettingParameterT<SettingParameter>
    {
        SettingParameter(CelestiaWinUI::SettingWindowProvider const& windowProvider);
        CelestiaWinUI::SettingWindowProvider WindowProvider();

    private:
        CelestiaWinUI::SettingWindowProvider windowProvider;
    };

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
        Microsoft::UI::Xaml::DataTemplate Info();
        void Info(Microsoft::UI::Xaml::DataTemplate const&);
        Microsoft::UI::Xaml::DataTemplate Separator();
        void Separator(Microsoft::UI::Xaml::DataTemplate const&);
        Microsoft::UI::Xaml::DataTemplate DataDirectory();
        void DataDirectory(Microsoft::UI::Xaml::DataTemplate const&);
        Microsoft::UI::Xaml::DataTemplate ConfigFile();
        void ConfigFile(Microsoft::UI::Xaml::DataTemplate const&);

        Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(Windows::Foundation::IInspectable const& item, Microsoft::UI::Xaml::DependencyObject const&);
        Microsoft::UI::Xaml::DataTemplate SelectTemplateCore(Windows::Foundation::IInspectable const& item);

    private:
        Microsoft::UI::Xaml::DataTemplate toggle{ nullptr };
        Microsoft::UI::Xaml::DataTemplate selection{ nullptr };
        Microsoft::UI::Xaml::DataTemplate slider{ nullptr };
        Microsoft::UI::Xaml::DataTemplate header{ nullptr };
        Microsoft::UI::Xaml::DataTemplate info{ nullptr };
        Microsoft::UI::Xaml::DataTemplate separator{ nullptr };
        Microsoft::UI::Xaml::DataTemplate dataDirectory{ nullptr };
        Microsoft::UI::Xaml::DataTemplate configFile{ nullptr };
    };

    struct SettingListItem : SettingListItemT<SettingListItem>
    {
        SettingListItem(Windows::Foundation::IInspectable const& item, bool isSetting);

        Windows::Foundation::IInspectable Item();
        Microsoft::UI::Xaml::Visibility SettingVisibility();
        Microsoft::UI::Xaml::Thickness BorderThickness();
        Microsoft::UI::Xaml::CornerRadius CornerRadius();
        Microsoft::UI::Xaml::Thickness Margin();
        Microsoft::UI::Xaml::Thickness ContentMargin();
        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

        void UpdateAppearance(Microsoft::UI::Xaml::Thickness const& borderThickness, Microsoft::UI::Xaml::CornerRadius const& cornerRadius, Microsoft::UI::Xaml::Thickness const& margin);

    private:
        Windows::Foundation::IInspectable item;
        bool isSetting;
        Microsoft::UI::Xaml::Thickness borderThickness;
        Microsoft::UI::Xaml::CornerRadius cornerRadius;
        Microsoft::UI::Xaml::Thickness margin;
        Microsoft::UI::Xaml::Thickness contentMargin;
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChangedEvent;
    };

    struct SettingGroupSeparator : SettingGroupSeparatorT<SettingGroupSeparator>
    {
        SettingGroupSeparator() = default;
    };

    struct SettingInfoItem : SettingInfoItemT<SettingInfoItem>
    {
        SettingInfoItem(hstring const& title);
        hstring Title();

    private:
        hstring title;
    };

    struct SettingCommonUserControl : SettingCommonUserControlT<SettingCommonUserControl>
    {
        SettingCommonUserControl(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& settingItems, bool showRestartHint, CelestiaWinUI::SettingParameter const& parameter);
        ~SettingCommonUserControl();
        void InitializeComponent();

        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::SettingListItem> Rows();
        bool ShowRestartHint();

        fire_and_forget DataDirectoryChangeButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void DataDirectoryResetButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const&);
        fire_and_forget ConfigFileChangeButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void ConfigFileResetButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> allItems;
        std::vector<CelestiaWinUI::SettingListItem> allRows;
        Windows::Foundation::Collections::IObservableVector<CelestiaWinUI::SettingListItem> rows;
        std::vector<std::pair<CelestiaAppComponent::SettingBaseItem, event_token>> visibilitySubscriptions;
        bool showRestartHint;
        CelestiaWinUI::SettingParameter parameter;

        void RefreshVisibleItems();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct SettingParameter : SettingParameterT<SettingParameter, implementation::SettingParameter>
    {
    };

    struct SettingTemplateSelector : SettingTemplateSelectorT<SettingTemplateSelector, implementation::SettingTemplateSelector>
    {
    };

    struct SettingListItem : SettingListItemT<SettingListItem, implementation::SettingListItem>
    {
    };

    struct SettingGroupSeparator : SettingGroupSeparatorT<SettingGroupSeparator, implementation::SettingGroupSeparator>
    {
    };

    struct SettingInfoItem : SettingInfoItemT<SettingInfoItem, implementation::SettingInfoItem>
    {
    };

    struct SettingCommonUserControl : SettingCommonUserControlT<SettingCommonUserControl, implementation::SettingCommonUserControl>
    {
    };
}
