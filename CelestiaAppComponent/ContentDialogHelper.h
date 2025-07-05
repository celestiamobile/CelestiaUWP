#pragma once

#include "ContentDialogHelper.g.h"
#include "completion_source.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct ContentDialogHelper : ContentDialogHelperT<ContentDialogHelper>
    {
        static Windows::Foundation::IAsyncAction ShowAlert(Microsoft::UI::Xaml::UIElement const element, hstring const message, hstring const content = L"");
        static Windows::Foundation::IAsyncOperation<bool> ShowOption(Microsoft::UI::Xaml::UIElement const element, hstring const message, hstring const content = L"");
        static Windows::Foundation::IAsyncOperation<hstring> GetText(Microsoft::UI::Xaml::UIElement const element, hstring const message, hstring const placeholder = L"");
        static Windows::Foundation::IAsyncAction ShowText(Microsoft::UI::Xaml::UIElement const element, hstring const title, hstring const text);
        static Windows::Foundation::IAsyncOperation<Microsoft::UI::Xaml::Controls::ContentDialogResult> ShowContentDialogAsync(Microsoft::UI::Xaml::UIElement const element, Microsoft::UI::Xaml::Controls::ContentDialog const contentDialog);
    private:
        static std::map<Microsoft::UI::Xaml::XamlRoot, std::vector<completion_source<bool>*>> pendingContentDialogRequests;

        static Windows::Foundation::IAsyncOperation<Microsoft::UI::Xaml::Controls::ContentDialogResult> ShowContentDialogDirectAsync(Microsoft::UI::Xaml::Controls::ContentDialog const contentDialog);
        static void NotifyNextContentDialog(Microsoft::UI::Xaml::XamlRoot const xamlRoot);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct ContentDialogHelper : ContentDialogHelperT<ContentDialogHelper, implementation::ContentDialogHelper>
    {
    };
}
