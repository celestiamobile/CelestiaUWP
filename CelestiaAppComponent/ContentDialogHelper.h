#pragma once

#include "ContentDialogHelper.g.h"
#include "completion_source.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct ContentDialogHelper : ContentDialogHelperT<ContentDialogHelper>
    {
        static Windows::Foundation::IAsyncAction ShowAlert(Windows::UI::Xaml::UIElement const element, hstring const message);
        static Windows::Foundation::IAsyncOperation<bool> ShowOption(Windows::UI::Xaml::UIElement const element, hstring const message);
        static Windows::Foundation::IAsyncOperation<hstring> GetText(Windows::UI::Xaml::UIElement const element, hstring const message);
        static Windows::Foundation::IAsyncAction ShowText(Windows::UI::Xaml::UIElement const element, hstring const title, hstring const text);
        static Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Controls::ContentDialogResult> ShowContentDialogAsync(Windows::UI::Xaml::UIElement const element, Windows::UI::Xaml::Controls::ContentDialog const contentDialog);
    private:
        static std::map<Windows::UI::Xaml::XamlRoot, std::vector<completion_source<bool>*>> pendingContentDialogRequests;

        static Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Controls::ContentDialogResult> ShowContentDialogDirectAsync(Windows::UI::Xaml::Controls::ContentDialog const contentDialog);
        static void NotifyNextContentDialog(Windows::UI::Xaml::XamlRoot const xamlRoot);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct ContentDialogHelper : ContentDialogHelperT<ContentDialogHelper, implementation::ContentDialogHelper>
    {
    };
}
