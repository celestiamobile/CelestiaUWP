#include "pch.h"
#include "CustomPage.xaml.h"
#if __has_include("CustomPage.g.cpp")
#include "CustomPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::CelestiaWinUI::implementation
{
    CustomPage::CustomPage()
    {
    }

    void CustomPage::OnNavigatedTo(Navigation::NavigationEventArgs const& args)
    {
        auto customView = args.Parameter().try_as<UIElement>();
        if (customView != nullptr)
        {
            Content(customView);
        }
    }
}
