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
        actions = single_threaded_observable_vector<BrowserAction>
        ({
            BrowserInputAction(LocalizationHelper::Localize(L"Go", L"Go to an object"), 103),
            BrowserInputAction(LocalizationHelper::Localize(L"Follow", L""), 102),
            BrowserInputAction(LocalizationHelper::Localize(L"Sync Orbit", L""), 121),
            BrowserInputAction(LocalizationHelper::Localize(L"Lock Phase", L""), 58),
            BrowserInputAction(LocalizationHelper::Localize(L"Chase", L""), 34),
            BrowserInputAction(LocalizationHelper::Localize(L"Track", L"Track an object"), 116),
            BrowserShowSubsystemAction(),
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

    Collections::IObservableVector<BrowserAction> InfoUserControl::Actions()
    {
        return actions;
    }

    void InfoUserControl::ActionButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto action = sender.as<Button>().DataContext().as<BrowserAction>();
        if (action.try_as<BrowserShowSubsystemAction>() != nullptr)
        {
            auto args = make<InfoShowSubsystemArgs>(selection);
            showSubsystemEvent(*this, args);
            if (!args.Handled())
            {
                CelestiaAppCore core = appCore;
                auto items = single_threaded_observable_vector<BrowserItem>();
                CelestiaBrowserItem browserItem{ appCore.Simulation().Universe().NameForSelection(selection), selection.Object(), [core](CelestiaBrowserItem const& item)
                    {
                        return CelestiaExtension::GetChildren(item, core);
                    }, false };
                items.Append(BrowserItem(browserItem));
                BrowserItemUserControl userControl{ appCore, renderer, BrowserItemTab(items, L"") };
                Window window;
                window.SystemBackdrop(Media::MicaBackdrop());
                window.Content(userControl);
                window.Title(appCore.Simulation().Universe().NameForSelection(selection));
                WindowHelper::SetWindowIcon(window);
                WindowHelper::SetWindowTheme(window);
                WindowHelper::SetWindowFlowDirection(window);
                WindowHelper::ResizeWindow(window, 600, 400);
                window.Activate();
            }
        }
        else if (auto inputAction = action.try_as<BrowserInputAction>(); inputAction != nullptr)
        {
            renderer.EnqueueTask([weak_this{ get_weak() }, inputAction]()
                {
                    auto strong_this{ weak_this.get() };
                    if (strong_this == nullptr) return;
                    strong_this->appCore.Simulation().Selection(strong_this->selection);
                    strong_this->appCore.CharEnter(inputAction.Code());
                });
        }
    }

    event_token InfoUserControl::ShowSubsystem(Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs> const& handler)
    {
        return showSubsystemEvent.add(handler);
    }

    void InfoUserControl::ShowSubsystem(event_token const& token) noexcept
    {
        showSubsystemEvent.remove(token);
    }
}