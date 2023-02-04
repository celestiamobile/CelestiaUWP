// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "AboutDialog.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct AboutDialog : AboutDialogT<AboutDialog>
    {
        AboutDialog(hstring const& authorsFilePath, hstring const& translatorsFilePath);

    private:
        fire_and_forget LoadAuthorTranslator(hstring const authorsFilePath, hstring const translatorsFilePath);
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct AboutDialog : AboutDialogT<AboutDialog, implementation::AboutDialog>
    {
    };
}
