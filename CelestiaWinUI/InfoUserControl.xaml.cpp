#include "pch.h"
#include "InfoUserControl.xaml.h"
#if __has_include("InfoUserControl.g.cpp")
#include "InfoUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    InfoUserControl::InfoUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, CelestiaSelection const& selection, bool preserveMargin) : appCore(appCore), renderer(renderer), selection(selection), preserveMargin(preserveMargin)
    {
        actions = single_threaded_observable_vector<BrowserInputAction>
        ({
            BrowserInputAction(LocalizationHelper::Localize(L"Go", L"Go to an object"), 103),
            BrowserInputAction(LocalizationHelper::Localize(L"Follow", L""), 102),
            BrowserInputAction(LocalizationHelper::Localize(L"Sync Orbit", L""), 121),
            BrowserInputAction(LocalizationHelper::Localize(L"Lock Phase", L""), 58),
            BrowserInputAction(LocalizationHelper::Localize(L"Chase", L""), 34),
            BrowserInputAction(LocalizationHelper::Localize(L"Track", L"Track an object"), 116)
        });
    }

    void InfoUserControl::InitializeComponent()
    {
        InfoUserControlT::InitializeComponent();

        if (!preserveMargin)
        {
            Thickness margin{ 0, 0, 0, 0 };
            Container().Margin(margin);
        }

        auto obj = selection.Object();
        if (obj == nullptr) return;

        renderer.EnqueueTask([weak_this{ get_weak() }, obj]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                auto name = strong_this->appCore.Simulation().Universe().NameForSelection(strong_this->selection);
                auto detail = SelectionHelper::GetOverview(strong_this->selection, strong_this->appCore);
                auto url = strong_this->selection.InfoURL();
                strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [strong_this, name, detail, url]()
                    {
                        strong_this->NameLabel().Text(name);
                        strong_this->DetailLabel().Text(detail);
                        if (!url.empty())
                        {
                            strong_this->LinkButton().NavigateUri(Uri(url));
                            strong_this->LinkButton().Content(box_value(url));
                            strong_this->LinkButton().Visibility(Visibility::Visible);
                        }
                        else
                        {
                            strong_this->LinkButton().Visibility(Visibility::Collapsed);
                        }
                    });
            });
    }

    Collections::IObservableVector<BrowserInputAction> InfoUserControl::Actions()
    {
        return actions;
    }

    void InfoUserControl::ActionButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto action = sender.as<Button>().DataContext().as<BrowserInputAction>();
        renderer.EnqueueTask([weak_this{ get_weak() }, action]()
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->appCore.Simulation().Selection(strong_this->selection);
                strong_this->appCore.CharEnter(action.Code());
            });
    }
}