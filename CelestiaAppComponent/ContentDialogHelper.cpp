#include "pch.h"
#include "ContentDialogHelper.h"
#if __has_include("ContentDialogHelper.g.cpp")
#include "ContentDialogHelper.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaAppComponent::implementation
{
    std::map<XamlRoot, std::vector<completion_source<bool>*>> ContentDialogHelper::pendingContentDialogRequests;

    IAsyncAction ContentDialogHelper::ShowAlert(UIElement const element, hstring const message)
    {
        ContentDialog dialog;
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.Title(box_value(message));
        dialog.PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        dialog.IsSecondaryButtonEnabled(false);
        co_await ShowContentDialogAsync(element, dialog);
    }

    IAsyncOperation<bool> ContentDialogHelper::ShowOption(UIElement const element, hstring const message)
    {
        ContentDialog dialog;
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.Title(box_value(message));
        dialog.PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        dialog.SecondaryButtonText(LocalizationHelper::Localize(L"Cancel"));
        dialog.IsSecondaryButtonEnabled(true);
        auto result{ co_await ShowContentDialogAsync(element, dialog) };
        co_return result == ContentDialogResult::Primary;
    }

    IAsyncOperation<hstring> ContentDialogHelper::GetText(UIElement const element, hstring const message)
    {
        ContentDialog dialog;
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.Title(box_value(message));
        dialog.PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        dialog.SecondaryButtonText(LocalizationHelper::Localize(L"Cancel"));
        dialog.IsSecondaryButtonEnabled(true);
        TextBox textBox;
        dialog.Content(textBox);

        auto result{ co_await ShowContentDialogAsync(element, dialog) };
        if (result == ContentDialogResult::Primary)
            co_return textBox.Text();
        co_return L"";
    }

    IAsyncAction ContentDialogHelper::ShowText(UIElement const element, hstring const title, hstring const text)
    {
        ContentDialog dialog;
        dialog.DefaultButton(ContentDialogButton::Primary);
        dialog.Title(box_value(title));
        dialog.PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        dialog.IsSecondaryButtonEnabled(false);
        TextBox textBox;
        textBox.TextWrapping(TextWrapping::Wrap);
        textBox.IsReadOnly(true);
        textBox.Text(text);
        dialog.Content(textBox);
        co_await ShowContentDialogAsync(element, dialog);
    }

    IAsyncOperation<ContentDialogResult> ContentDialogHelper::ShowContentDialogAsync(UIElement const element, ContentDialog const contentDialog)
    {
        winrt::apartment_context context;
        auto xamlRoot = element.XamlRoot();
        auto result = pendingContentDialogRequests.find(xamlRoot);
        if (result == pendingContentDialogRequests.end())
        {
            contentDialog.XamlRoot(xamlRoot);
            pendingContentDialogRequests[xamlRoot] = {};
            auto dialogResult{ co_await ShowContentDialogDirectAsync(contentDialog) };
            NotifyNextContentDialog(xamlRoot);
            co_return dialogResult;
        }
        else
        {
            completion_source<bool> completionSource;
            pendingContentDialogRequests[xamlRoot].push_back(&completionSource);
            co_await completionSource;
            co_await context;
            contentDialog.XamlRoot(xamlRoot);
            co_await ShowContentDialogDirectAsync(contentDialog);
            NotifyNextContentDialog(xamlRoot);
        }
    }

    void ContentDialogHelper::NotifyNextContentDialog(XamlRoot const xamlRoot)
    {
        auto result = pendingContentDialogRequests.find(xamlRoot);
        if (result != pendingContentDialogRequests.end())
        {
            auto& pendingRequests = result->second;
            if (pendingRequests.size() > 0)
            {
                auto pendingRequest = pendingRequests[0];
                pendingRequests.erase(pendingRequests.begin());
                pendingRequest->set(true);
            }
            else
            {
                pendingContentDialogRequests.erase(result);
            }
        }
    }

    IAsyncOperation<ContentDialogResult> ContentDialogHelper::ShowContentDialogDirectAsync(ContentDialog const contentDialog)
    {
        co_return co_await contentDialog.ShowAsync();
    }
}
