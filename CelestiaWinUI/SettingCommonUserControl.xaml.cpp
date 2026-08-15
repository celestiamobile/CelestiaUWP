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
#if __has_include("SettingItemGroup.g.cpp")
#include "SettingItemGroup.g.cpp"
#endif
#if __has_include("SettingGroupSeparator.g.cpp")
#include "SettingGroupSeparator.g.cpp"
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
        if (item.try_as<SettingDataDirectoryItem>() != nullptr) return dataDirectory;
        if (item.try_as<SettingConfigFileItem>() != nullptr) return configFile;
        return nullptr;
    }

    SettingItemGroup::SettingItemGroup(hstring const& title, hstring const& description, bool headerVisible, bool descriptionVisible) :
        title(title),
        description(description),
        headerVisible(headerVisible),
        descriptionVisible(descriptionVisible),
        items(single_threaded_observable_vector<IInspectable>())
    {
    }

    hstring SettingItemGroup::Title()
    {
        return title;
    }

    hstring SettingItemGroup::Description()
    {
        return description;
    }

    bool SettingItemGroup::HeaderVisible()
    {
        return headerVisible;
    }

    void SettingItemGroup::HeaderVisible(bool value)
    {
        if (headerVisible == value)
            return;
        headerVisible = value;
        propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"HeaderVisible"));
    }

    bool SettingItemGroup::DescriptionVisible()
    {
        return descriptionVisible;
    }

    Collections::IObservableVector<IInspectable> SettingItemGroup::Items()
    {
        return items;
    }

    event_token SettingItemGroup::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChangedEvent.add(handler);
    }

    void SettingItemGroup::PropertyChanged(event_token const& token) noexcept
    {
        propertyChangedEvent.remove(token);
    }

    SettingCommonUserControl::SettingCommonUserControl(Collections::IObservableVector<IInspectable> const& settingItems, bool showRestartHint, CelestiaWinUI::SettingParameter const& parameter) : allItems(settingItems), groups(single_threaded_observable_vector<CelestiaWinUI::SettingItemGroup>()), showRestartHint(showRestartHint), parameter(parameter)
    {
        for (auto const& item : allItems)
        {
            if (item.try_as<CelestiaWinUI::SettingGroupSeparator>())
            {
                auto group = CelestiaWinUI::SettingItemGroup(L"", L"", false, false);
                allGroups.push_back({ nullptr, {}, group });
                continue;
            }
            else if (auto header = item.try_as<SettingHeaderItem>())
            {
                auto group = CelestiaWinUI::SettingItemGroup(header.Title(), header.Description(), true, header.DescriptionVisibility());
                allGroups.push_back({ header, {}, group });
            }
            else
            {
                if (allGroups.empty())
                {
                    auto group = CelestiaWinUI::SettingItemGroup(L"", L"", false, false);
                    allGroups.push_back({ nullptr, {}, group });
                }
                allGroups.back().allItems.push_back(item);
            }

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
        uint32_t visibleGroupIndex = 0;
        for (auto const& state : allGroups)
        {
            auto visibleItems = state.group.Items();
            uint32_t visibleItemIndex = 0;
            for (auto const& item : state.allItems)
            {
                auto settingItem = item.try_as<SettingBaseItem>();
                auto isVisible = !settingItem || settingItem.IsVisible();
                uint32_t currentIndex;
                auto isDisplayed = visibleItems.IndexOf(item, currentIndex);

                if (!isVisible)
                {
                    if (isDisplayed)
                        visibleItems.RemoveAt(currentIndex);
                    continue;
                }

                if (!isDisplayed)
                {
                    visibleItems.InsertAt(visibleItemIndex, item);
                }
                else if (currentIndex != visibleItemIndex)
                {
                    visibleItems.RemoveAt(currentIndex);
                    visibleItems.InsertAt(visibleItemIndex, item);
                }
                ++visibleItemIndex;
            }

            state.group.HeaderVisible(state.header && state.header.IsVisible());
            auto isVisible = visibleItems.Size() > 0;
            uint32_t currentIndex;
            auto isDisplayed = groups.IndexOf(state.group, currentIndex);
            if (!isVisible)
            {
                if (isDisplayed)
                    groups.RemoveAt(currentIndex);
                continue;
            }

            if (!isDisplayed)
            {
                groups.InsertAt(visibleGroupIndex, state.group);
            }
            else if (currentIndex != visibleGroupIndex)
            {
                groups.RemoveAt(currentIndex);
                groups.InsertAt(visibleGroupIndex, state.group);
            }
            ++visibleGroupIndex;
        }
    }

    void SettingCommonUserControl::InitializeComponent()
    {
        SettingCommonUserControlT::InitializeComponent();
        RestartHint().Title(LocalizationHelper::Localize(L"Some configurations will take effect after a restart.", L""));
    }

    Collections::IObservableVector<CelestiaWinUI::SettingItemGroup> SettingCommonUserControl::Groups()
    {
        return groups;
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