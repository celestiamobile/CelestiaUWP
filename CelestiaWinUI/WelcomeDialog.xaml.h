// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "WelcomeDialog.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct WelcomeDialog : WelcomeDialogT<WelcomeDialog>
    {
        WelcomeDialog();

        bool ShouldNotShowMessageAgain();
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct WelcomeDialog : WelcomeDialogT<WelcomeDialog, implementation::WelcomeDialog>
    {
    };
}
