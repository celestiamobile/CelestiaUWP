// SafeWebUserControl.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "SafeWebUserControl.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct SafeWebUserControl : SafeWebUserControlT<SafeWebUserControl>
    {
        SafeWebUserControl(CelestiaWinUI::CommonWebParameter const& args);
        void InitializeComponent();

        void CloseWebViewIfNeeded();
    private:
        CelestiaWinUI::CommonWebParameter args;
        CelestiaWinUI::CommonWebUserControl webPage{ nullptr };
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct SafeWebUserControl : SafeWebUserControlT<SafeWebUserControl, implementation::SafeWebUserControl>
    {
    };
}
