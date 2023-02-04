#include "pch.h"
#include "SafeWebUserControl.xaml.h"
#if __has_include("SafeWebUserControl.g.cpp")
#include "SafeWebUserControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::CelestiaWinUI::implementation
{
    SafeWebUserControl::SafeWebUserControl(CelestiaWinUI::CommonWebUserControlArgs const& args)
    {
        InitializeComponent();

        hstring webViewVersion = L"";
        try
        {
            webViewVersion = Microsoft::Web::WebView2::Core::CoreWebView2Environment::GetAvailableBrowserVersionString();
        }
        catch(hresult_error const&) {}
        if (webViewVersion.empty())
        {
            WebViewNotFoundView().Visibility(Visibility::Visible);
        }
        else
        {
            WebContent().Visibility(Visibility::Visible);
            webPage = CelestiaWinUI::CommonWebUserControl(args);
            WebContent().Children().Append(webPage);
            WebContent().SetAlignTopWithPanel(webPage, true);
            WebContent().SetAlignBottomWithPanel(webPage, true);
            WebContent().SetAlignLeftWithPanel(webPage, true);
            WebContent().SetAlignRightWithPanel(webPage, true);
        }
    }

    void SafeWebUserControl::CloseWebViewIfNeeded()
    {
        if (webPage == nullptr) return;
        webPage.CloseWebViewIfNeeded();
    }
}
