#include "pch.h"
#include "InfoUserControl.xaml.h"
#if __has_include("InfoUserControl.g.cpp")
#include "InfoUserControl.g.cpp"
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
    InfoUserControl::InfoUserControl(CelestiaAppCore const& appCore, CelestiaSelection const& selection)
    {
        InitializeComponent();
        auto obj = selection.Object();
        if (obj == nullptr) return;
        NameLabel().Text(appCore.Simulation().Universe().NameForSelection(selection));
        DetailLabel().Text(SelectionHelper::GetOverview(selection, appCore));
        auto url = selection.InfoURL();
        if (!url.empty())
        {
            LinkButton().NavigateUri(Uri(url));
            LinkButton().Content(box_value(url));
        }
    }
}