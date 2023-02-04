// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

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
