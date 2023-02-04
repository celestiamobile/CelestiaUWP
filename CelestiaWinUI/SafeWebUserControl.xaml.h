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
