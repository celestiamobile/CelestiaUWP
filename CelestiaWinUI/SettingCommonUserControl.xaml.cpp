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
        if (item.try_as<SettingDataDirectoryItem>() != nullptr) return dataDirectory;
        if (item.try_as<SettingConfigFileItem>() != nullptr) return configFile;
        return nullptr;
    }

    SettingCommonUserControl::SettingCommonUserControl(Collections::IObservableVector<IInspectable> const& settingItems, bool showRestartHint, CelestiaWinUI::SettingParameter const& parameter) : items(settingItems), showRestartHint(showRestartHint), parameter(parameter)
    {
    }

    void SettingCommonUserControl::InitializeComponent()
    {
        SettingCommonUserControlT::InitializeComponent();
        RestartHint().Title(LocalizationHelper::Localize(L"Some configurations will take effect after a restart.", L""));
    }

    Collections::IObservableVector<IInspectable> SettingCommonUserControl::Items()
    {
        return items;
    }

    bool SettingCommonUserControl::ShowRestartHint()
    {
        return showRestartHint;
    }

    fire_and_forget SettingCommonUserControl::DataDirectoryChangeButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto button = sender.as<Controls::Button>();
        auto item = button.DataContext().as<SettingDataDirectoryItem>();
        if (parameter == nullptr) return;
        auto window = parameter.WindowProvider()();
        if (window == nullptr) return;

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
        if (parameter == nullptr) return;
        auto window = parameter.WindowProvider()();
        if (window == nullptr) return;

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