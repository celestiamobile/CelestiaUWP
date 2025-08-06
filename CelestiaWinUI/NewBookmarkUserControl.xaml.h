// NewBookmarkUserControl.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "NewBookmarkUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct NewBookmarkUserControl : NewBookmarkUserControlT<NewBookmarkUserControl>
    {
        NewBookmarkUserControl(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        void InitializeComponent();

        hstring NameText();
        void NameText(hstring const&);

        void ConfirmButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        hstring nameText{ L"" };
        CelestiaWinUI::BookmarkOrganizerUserControl organizer;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct NewBookmarkUserControl : NewBookmarkUserControlT<NewBookmarkUserControl, implementation::NewBookmarkUserControl>
    {
    };
}
