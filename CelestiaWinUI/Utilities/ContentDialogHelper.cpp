#include "pch.h"
#include "ContentDialogHelper.h"
#if __has_include("ContentDialogHelper.g.cpp")
#include "ContentDialogHelper.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    IAsyncAction ContentDialogHelper::ShowAlert(UIElement const element, hstring const message)
    {
        ContentDialog dialog;
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.XamlRoot(element.XamlRoot());
        dialog.Title(box_value(message));
        dialog.PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        dialog.IsSecondaryButtonEnabled(false);
        co_await dialog.ShowAsync();
        co_return;
    }

    IAsyncOperation<bool> ContentDialogHelper::ShowOption(UIElement const element, hstring const message)
    {
        ContentDialog dialog;
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.XamlRoot(element.XamlRoot());
        dialog.Title(box_value(message));
        dialog.PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        dialog.SecondaryButtonText(LocalizationHelper::Localize(L"Cancel"));
        dialog.IsSecondaryButtonEnabled(true);
        auto result{ co_await dialog.ShowAsync() };
        co_return result == ContentDialogResult::Primary;
    }

    IAsyncOperation<hstring> ContentDialogHelper::GetText(UIElement const element, hstring const message)
    {
        ContentDialog dialog;
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.XamlRoot(element.XamlRoot());
        dialog.Title(box_value(message));
        dialog.PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        dialog.SecondaryButtonText(LocalizationHelper::Localize(L"Cancel"));
        dialog.IsSecondaryButtonEnabled(true);
        TextBox textBox;
        dialog.Content(textBox);

        auto result{ co_await dialog.ShowAsync() };
        if (result == ContentDialogResult::Primary)
            co_return textBox.Text();
        co_return L"";
    }

    IAsyncAction ContentDialogHelper::ShowText(UIElement const element, hstring const title, hstring const text)
    {
        ContentDialog dialog;
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.XamlRoot(element.XamlRoot());
        dialog.Title(box_value(title));
        dialog.PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        dialog.IsSecondaryButtonEnabled(false);
        TextBox textBox;
        textBox.TextWrapping(TextWrapping::Wrap);
        textBox.IsReadOnly(true);
        textBox.Text(text);
        dialog.Content(textBox);
        co_await dialog.ShowAsync();
        co_return;
    }
}
