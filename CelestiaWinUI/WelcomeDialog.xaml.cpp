#include "pch.h"
#include "WelcomeDialog.xaml.h"
#if __has_include("WelcomeDialog.g.cpp")
#include "WelcomeDialog.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Documents;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    WelcomeDialog::WelcomeDialog()
    {
        InitializeComponent();
        PrimaryButtonText(LocalizationHelper::Localize(L"OK"));

        auto infoText = static_cast<std::wstring>(LocalizationHelper::Localize(L"Welcome to Celestia. To learn more about using Celestia on Xbox, please visit here."));
        auto linkInfoText = static_cast<std::wstring>(LocalizationHelper::Localize(L"here"));
        auto pos = infoText.find(linkInfoText);

        if (pos != std::wstring::npos)
        {
            Run textBefore;
            textBefore.Text(infoText.substr(0, pos));
            Hyperlink link;
            Run linkRun;
            linkRun.Text(linkInfoText);
            link.Inlines().Append(linkRun);

            Windows::Web::Http::HttpFormUrlEncodedContent query({ { L"lang", LocalizationHelper::Locale()} });
            hstring url = hstring(L"https://celestia.mobi/help/xbox") + L"?" + query.ToString();
            link.NavigateUri(Uri(url));

            Run textAfter;
            textAfter.Text(infoText.substr(pos + linkInfoText.size()));
            WelcomeMessageContent().Inlines().Append(textBefore);
            WelcomeMessageContent().Inlines().Append(link);
            WelcomeMessageContent().Inlines().Append(textAfter);
        }
        else
        {
            WelcomeMessageContent().Visibility(Visibility::Collapsed);
        }
        IgnoreCheckBox().Content(box_value(LocalizationHelper::Localize(L"Don't show this message again")));
    }

    bool WelcomeDialog::ShouldNotShowMessageAgain()
    {
        auto checked = IgnoreCheckBox().IsChecked();
        return unbox_value_or(checked, false);
    }
}
