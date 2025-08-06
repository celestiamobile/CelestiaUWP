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
using namespace Microsoft::UI::Xaml;

namespace winrt::CelestiaWinUI::implementation
{
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
        return nullptr;
    }

    SettingCommonUserControl::SettingCommonUserControl(Collections::IObservableVector<IInspectable> const& settingItems, bool showRestartHint) : items(settingItems), showRestartHint(showRestartHint)
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
}
