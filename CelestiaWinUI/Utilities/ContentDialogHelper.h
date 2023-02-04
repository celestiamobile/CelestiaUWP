#pragma once

#include "ContentDialogHelper.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct ContentDialogHelper : ContentDialogHelperT<ContentDialogHelper>
    {
        static Windows::Foundation::IAsyncAction ShowAlert(Microsoft::UI::Xaml::UIElement const element, hstring const message);
        static Windows::Foundation::IAsyncOperation<bool> ShowOption(Microsoft::UI::Xaml::UIElement const element, hstring const message);
        static Windows::Foundation::IAsyncOperation<hstring> GetText(Microsoft::UI::Xaml::UIElement const element, hstring const message);
        static Windows::Foundation::IAsyncAction ShowText(Microsoft::UI::Xaml::UIElement const element, hstring const title, hstring const text);
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct ContentDialogHelper : ContentDialogHelperT<ContentDialogHelper, implementation::ContentDialogHelper>
    {
    };
}
