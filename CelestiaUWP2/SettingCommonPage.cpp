//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "SettingCommonPage.h"

#if __has_include("SettingCommonParameter.g.cpp")
#include "SettingCommonParameter.g.cpp"
#endif
#if __has_include("SettingTemplateSelector.g.cpp")
#include "SettingTemplateSelector.g.cpp"
#endif
#if __has_include("SettingCommonPage.g.cpp")
#include "SettingCommonPage.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

namespace winrt::CelestiaUWP2::implementation
{
    SettingCommonParameter::SettingCommonParameter(Collections::IObservableVector<IInspectable> const& settingItems, bool showRestartHint) : settingItems(settingItems), showRestartHint(showRestartHint) {}
    Collections::IObservableVector<IInspectable> SettingCommonParameter::Items() { return settingItems; }
    bool SettingCommonParameter::ShowRestartHint() { return showRestartHint; }

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

    SettingCommonPage::SettingCommonPage() : items(nullptr), showRestartHint(false)
    {
    }

    void SettingCommonPage::InitializeComponent()
    {
        SettingCommonPageT::InitializeComponent();
        RestartHint().Title(LocalizationHelper::Localize(L"Some configurations will take effect after a restart.", L""));
    }

    Collections::IObservableVector<IInspectable> SettingCommonPage::Items()
    {
        return items;
    }

    bool SettingCommonPage::ShowRestartHint()
    {
        return showRestartHint;
    }

    void SettingCommonPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto parameter = e.Parameter().as<CelestiaUWP2::SettingCommonParameter>();
        items = parameter.Items();
        showRestartHint = parameter.ShowRestartHint();
    }
}
