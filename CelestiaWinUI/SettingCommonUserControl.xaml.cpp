// SettingCommonUserControl.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "SettingCommonUserControl.xaml.h"
#if __has_include("SettingParameter.g.cpp")
#include "SettingParameter.g.cpp"
#endif
#if __has_include("SettingListItem.g.cpp")
#include "SettingListItem.g.cpp"
#endif
#if __has_include("SettingGroupSeparator.g.cpp")
#include "SettingGroupSeparator.g.cpp"
#endif
#if __has_include("SettingInfoItem.g.cpp")
#include "SettingInfoItem.g.cpp"
#endif
#if __has_include("SettingCommonUserControl.g.cpp")
#include "SettingCommonUserControl.g.cpp"
#endif
#if __has_include("SettingTemplateSelector.g.cpp")
#include "SettingTemplateSelector.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::Windows::Storage::Pickers;

namespace winrt::CelestiaWinUI::implementation
{
    SettingParameter::SettingParameter(CelestiaWinUI::SettingWindowProvider const& windowProvider): windowProvider(windowProvider)
    {
    }

    CelestiaWinUI::SettingWindowProvider SettingParameter::WindowProvider()
    {
        return windowProvider;
    }

    SettingTemplateSelector::SettingTemplateSelector()
    {
    }

    DataTemplate SettingTemplateSelector::Toggle()
    {
        return toggle;
    }

    void SettingTemplateSelector::Toggle(DataTemplate const& value)
    {
        toggle = value;
    }

    DataTemplate SettingTemplateSelector::Selection()
    {
        return selection;
    }

    void SettingTemplateSelector::Selection(DataTemplate const& value)
    {
        selection = value;
    }

    DataTemplate SettingTemplateSelector::Slider()
    {
        return slider;
    }

    void SettingTemplateSelector::Slider(DataTemplate const& value)
    {
        slider = value;
    }

    DataTemplate SettingTemplateSelector::Header()
    {
        return header;
    }

    void SettingTemplateSelector::Header(DataTemplate const& value)
    {
        header = value;
    }

    DataTemplate SettingTemplateSelector::Info()
    {
        return info;
    }

    void SettingTemplateSelector::Info(DataTemplate const& value)
    {
        info = value;
    }

    DataTemplate SettingTemplateSelector::Separator()
    {
        return separator;
    }

    void SettingTemplateSelector::Separator(DataTemplate const& value)
    {
        separator = value;
    }

    DataTemplate SettingTemplateSelector::DataDirectory()
    {
        return dataDirectory;
    }

    void SettingTemplateSelector::DataDirectory(DataTemplate const& value)
    {
        dataDirectory = value;
    }

    DataTemplate SettingTemplateSelector::ConfigFile()
    {
        return configFile;
    }

    void SettingTemplateSelector::ConfigFile(DataTemplate const& value)
    {
        configFile = value;
    }

    DataTemplate SettingTemplateSelector::SelectTemplateCore(IInspectable const& item, DependencyObject const&)
    {
        return SelectTemplateCore(item);
    }

    DataTemplate SettingTemplateSelector::SelectTemplateCore(IInspectable const& item)
    {
        if (item.try_as<SettingBooleanItem>() != nullptr) return toggle;
        if (item.try_as<SettingInt32Item>() != nullptr) return selection;
        if (item.try_as<SettingDoubleItem>() != nullptr) return slider;
        if (item.try_as<SettingHeaderItem>() != nullptr) return header;
        if (item.try_as<CelestiaWinUI::SettingInfoItem>() != nullptr) return info;
        if (item.try_as<CelestiaWinUI::SettingGroupSeparator>() != nullptr) return separator;
        if (item.try_as<SettingDataDirectoryItem>() != nullptr) return dataDirectory;
        if (item.try_as<SettingConfigFileItem>() != nullptr) return configFile;
        return nullptr;
    }

    SettingInfoItem::SettingInfoItem(hstring const& title) : title(title)
    {
    }

    hstring SettingInfoItem::Title()
    {
        return title;
    }

    SettingListItem::SettingListItem(IInspectable const& item, bool isSetting) :
        item(item),
        isSetting(isSetting),
        borderThickness{},
        cornerRadius{},
        margin{},
        contentMargin(isSetting ? Thickness{ 16, 16, 16, 16 } : Thickness{})
    {
    }

    IInspectable SettingListItem::Item()
    {
        return item;
    }

    Visibility SettingListItem::SettingVisibility()
    {
        return isSetting ? Visibility::Visible : Visibility::Collapsed;
    }

    Thickness SettingListItem::BorderThickness()
    {
        return borderThickness;
    }

    Microsoft::UI::Xaml::CornerRadius SettingListItem::CornerRadius()
    {
        return cornerRadius;
    }

    Thickness SettingListItem::Margin()
    {
        return margin;
    }

    Thickness SettingListItem::ContentMargin()
    {
        return contentMargin;
    }

    event_token SettingListItem::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChangedEvent.add(handler);
    }

    void SettingListItem::PropertyChanged(event_token const& token) noexcept
    {
        propertyChangedEvent.remove(token);
    }

    void SettingListItem::UpdateAppearance(Thickness const& valueBorderThickness, Microsoft::UI::Xaml::CornerRadius const& valueCornerRadius, Thickness const& valueMargin)
    {
        if (borderThickness.Left != valueBorderThickness.Left ||
            borderThickness.Top != valueBorderThickness.Top ||
            borderThickness.Right != valueBorderThickness.Right ||
            borderThickness.Bottom != valueBorderThickness.Bottom)
        {
            borderThickness = valueBorderThickness;
            propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"BorderThickness"));
        }
        if (cornerRadius.TopLeft != valueCornerRadius.TopLeft ||
            cornerRadius.TopRight != valueCornerRadius.TopRight ||
            cornerRadius.BottomRight != valueCornerRadius.BottomRight ||
            cornerRadius.BottomLeft != valueCornerRadius.BottomLeft)
        {
            cornerRadius = valueCornerRadius;
            propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"CornerRadius"));
        }
        if (margin.Left != valueMargin.Left ||
            margin.Top != valueMargin.Top ||
            margin.Right != valueMargin.Right ||
            margin.Bottom != valueMargin.Bottom)
        {
            margin = valueMargin;
            propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"Margin"));
        }
    }

    SettingCommonUserControl::SettingCommonUserControl(Collections::IObservableVector<IInspectable> const& settingItems, bool showRestartHint, CelestiaWinUI::SettingParameter const& parameter) : allItems(settingItems), rows(single_threaded_observable_vector<CelestiaWinUI::SettingListItem>()), showRestartHint(showRestartHint), parameter(parameter)
    {
        for (auto const& item : allItems)
        {
            auto isSetting = item.try_as<SettingBooleanItem>() ||
                item.try_as<SettingInt32Item>() ||
                item.try_as<SettingDoubleItem>() ||
                item.try_as<SettingDataDirectoryItem>() ||
                item.try_as<SettingConfigFileItem>();
            allRows.emplace_back(item, isSetting);
            auto settingItem = item.try_as<SettingBaseItem>();
            if (!settingItem)
                continue;
            auto token = settingItem.PropertyChanged([weak_this{ get_weak() }](IInspectable const&, Data::PropertyChangedEventArgs const& args)
                {
                    if (args.PropertyName() != L"IsVisible")
                        return;
                    if (auto strong_this = weak_this.get())
                        strong_this->RefreshVisibleItems();
                });
            visibilitySubscriptions.emplace_back(settingItem, token);
        }
        RefreshVisibleItems();
    }

    SettingCommonUserControl::~SettingCommonUserControl()
    {
        for (auto const& [item, token] : visibilitySubscriptions)
            item.PropertyChanged(token);
    }

    void SettingCommonUserControl::RefreshVisibleItems()
    {
        uint32_t visibleIndex = 0;
        for (auto const& row : allRows)
        {
            auto item = row.Item();
            auto settingItem = item.try_as<SettingBaseItem>();
            auto isVisible = !settingItem || settingItem.IsVisible();
            uint32_t currentIndex;
            auto isDisplayed = rows.IndexOf(row, currentIndex);

            if (!isVisible)
            {
                if (isDisplayed)
                    rows.RemoveAt(currentIndex);
                continue;
            }

            if (!isDisplayed)
            {
                rows.InsertAt(visibleIndex, row);
            }
            else if (currentIndex != visibleIndex)
            {
                rows.RemoveAt(currentIndex);
                rows.InsertAt(visibleIndex, row);
            }
            ++visibleIndex;
        }

        for (uint32_t index = 0; index < rows.Size(); ++index)
        {
            auto row = rows.GetAt(index);
            if (row.SettingVisibility() != Visibility::Visible)
                continue;

            auto previousIsSetting = index > 0 && rows.GetAt(index - 1).SettingVisibility() == Visibility::Visible;
            auto nextIsSetting = index + 1 < rows.Size() && rows.GetAt(index + 1).SettingVisibility() == Visibility::Visible;
            Thickness borderThickness = previousIsSetting ? Thickness{ 1, 0, 1, 1 } : Thickness{ 1, 1, 1, 1 };
            Microsoft::UI::Xaml::CornerRadius rowCornerRadius{};
            if (!previousIsSetting && !nextIsSetting)
                rowCornerRadius = { 4, 4, 4, 4 };
            else if (!previousIsSetting)
                rowCornerRadius = { 4, 4, 0, 0 };
            else if (!nextIsSetting)
                rowCornerRadius = { 0, 0, 4, 4 };

            auto topMargin = index == 0 ? 30.0 : 0.0;
            get_self<implementation::SettingListItem>(row)->UpdateAppearance(borderThickness, rowCornerRadius, { 0, topMargin, 0, 0 });
        }
    }

    void SettingCommonUserControl::InitializeComponent()
    {
        SettingCommonUserControlT::InitializeComponent();
        RestartHint().Title(LocalizationHelper::Localize(L"Some configurations will take effect after a restart.", L""));
    }

    Collections::IObservableVector<CelestiaWinUI::SettingListItem> SettingCommonUserControl::Rows()
    {
        return rows;
    }

    bool SettingCommonUserControl::ShowRestartHint()
    {
        return showRestartHint;
    }

    fire_and_forget SettingCommonUserControl::DataDirectoryChangeButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto button = sender.as<Controls::Button>();
        auto item = button.DataContext().as<SettingDataDirectoryItem>();
        if (parameter == nullptr) co_return;
        auto window = parameter.WindowProvider()();
        if (window == nullptr) co_return;

        FolderPicker picker{ window.AppWindow().Id() };
        picker.ViewMode(PickerViewMode::Thumbnail);
        picker.SuggestedStartLocation(PickerLocationId::Downloads);
        if (auto folderResult = co_await picker.PickSingleFolderAsync(); folderResult != nullptr)
        {
            item.Path(folderResult.Path());
        }
    }

    void SettingCommonUserControl::DataDirectoryResetButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto button = sender.as<Controls::Button>();
        auto item = button.DataContext().as<SettingDataDirectoryItem>();
        item.Path(L"");
    }

    fire_and_forget SettingCommonUserControl::ConfigFileChangeButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto button = sender.as<Controls::Button>();
        auto item = button.DataContext().as<SettingConfigFileItem>();
        if (parameter == nullptr) co_return;
        auto window = parameter.WindowProvider()();
        if (window == nullptr) co_return;

        FileOpenPicker picker{ window.AppWindow().Id() };
        picker.ViewMode(PickerViewMode::Thumbnail);
        picker.SuggestedStartLocation(PickerLocationId::Downloads);
        picker.FileTypeFilter().Append(L".cfg");
        if (auto fileResult = co_await picker.PickSingleFileAsync(); fileResult != nullptr)
        {
            item.Path(fileResult.Path());
        }
    }

    void SettingCommonUserControl::ConfigFileResetButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto button = sender.as<Controls::Button>();
        auto item = button.DataContext().as<SettingConfigFileItem>();
        item.Path(L"");
    }
}