// AboutDialog.xaml.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "AboutDialog.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct AboutDialog : AboutDialogT<AboutDialog>
    {
        AboutDialog(hstring const& authorsFilePath, hstring const& translatorsFilePath);
        void InitializeComponent();

    private:
        hstring authorsFilePath;
        hstring translatorsFilePath;

        fire_and_forget LoadAuthorTranslator();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct AboutDialog : AboutDialogT<AboutDialog, implementation::AboutDialog>
    {
    };
}
