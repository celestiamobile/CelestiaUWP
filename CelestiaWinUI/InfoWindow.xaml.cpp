#include "pch.h"
#include "InfoWindow.xaml.h"
#if __has_include("InfoWindow.g.cpp")
#include "InfoWindow.g.cpp"
#endif

#include <fmt/printf.h>
#include <fmt/xchar.h>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    InfoWindow::InfoWindow(CelestiaAppCore const& appCore, CelestiaSelection const& selection)
    {
        InitializeComponent();
        auto obj = selection.Object();
        if (obj == nullptr) return;
        NameLabel().Text(appCore.Simulation().Universe().NameForSelection(selection));
        DetailLabel().Text(CelestiaExtension::GetOverviewForSelection(selection, [](hstring const& original)
            {
                return LocalizationHelper::Localize(original);
            }, appCore));
        auto url = selection.InfoURL();
        if (!url.empty())
        {
            LinkButton().NavigateUri(Uri(url));
            LinkButton().Content(box_value(url));
        }
    }
}