#include "pch.h"
#include "BrowserItemUserControl.xaml.h"
#if __has_include("BrowserItemUserControl.g.cpp")
#include "BrowserItemUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::CelestiaWinUI::implementation
{
    BrowserItemUserControl::BrowserItemUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, BrowserItemTab const& item, bool preserveMargin) : appCore(appCore), renderer(renderer), preserveMargin(preserveMargin)
    {
        actions = single_threaded_observable_vector<BrowserAction>
        ({
            BrowserGetInfoAction(),
            BrowserInputAction(LocalizationHelper::Localize(L"Go", L"Go to an object"), 103),
            BrowserInputAction(LocalizationHelper::Localize(L"Follow", L""), 102),
            BrowserInputAction(LocalizationHelper::Localize(L"Sync Orbit", L""), 121),
            BrowserInputAction(LocalizationHelper::Localize(L"Lock Phase", L""), 58),
            BrowserInputAction(LocalizationHelper::Localize(L"Chase", L""), 34),
            BrowserInputAction(LocalizationHelper::Localize(L"Track", L"Track an object"), 116)
        });
        rootItem = BrowserItem::ConvertToBindable(item.Children());
    }

    void BrowserItemUserControl::InitializeComponent()
    {
        BrowserItemUserControlT::InitializeComponent();

        if (!preserveMargin)
        {
            Thickness margin{ 0, 0, 0, 0 };
            Container().Margin(margin);
        }
    }

    void BrowserItemUserControl::ActionButton_Click(IInspectable const& sender, RoutedEventArgs const&)
    {
        auto selectedItem = Tree().SelectedItem();
        if (selectedItem == nullptr) return;
        auto browserItem = selectedItem.try_as<BrowserItem>();
        if (browserItem == nullptr) return;
        auto object = browserItem.Item().Object();
        if (object == nullptr) return;
        auto selection = CelestiaSelection(object);
        auto context = sender.as<Button>().DataContext().as<BrowserAction>();

        if (context.try_as<BrowserGetInfoAction>() != nullptr)
        {
            auto args = make<BrowserItemGetInfoArgs>(selection);
            getInfoEvent(*this, args);
            if (!args.Handled())
            {
                InfoUserControl userControl{ appCore, renderer, selection };
                Window window;
                window.SystemBackdrop(Media::MicaBackdrop());
                window.Content(userControl);
                window.Title(appCore.Simulation().Universe().NameForSelection(selection));
                WindowHelper::SetWindowIcon(window);
                WindowHelper::SetWindowTheme(window);
                WindowHelper::SetWindowFlowDirection(window);
                WindowHelper::ResizeWindow(window, 700, 400);
                window.Activate();
            }
        }
        else if (auto inputAction = context.try_as<BrowserInputAction>(); inputAction != nullptr)
        {
            renderer.EnqueueTask([this, selection, inputAction]()
                {
                    appCore.Simulation().Selection(selection);
                    appCore.CharEnter(inputAction.Code());
                });
        }
    }

    Microsoft::UI::Xaml::Interop::IBindableObservableVector BrowserItemUserControl::RootItem()
    {
        return rootItem;
    }

    Collections::IObservableVector<BrowserAction> BrowserItemUserControl::Actions()
    {
        return actions;
    }

    event_token BrowserItemUserControl::GetInfo(Windows::Foundation::EventHandler<CelestiaWinUI::BrowserItemGetInfoArgs> const& handler)
    {
        return getInfoEvent.add(handler);
    }

    void BrowserItemUserControl::GetInfo(event_token const& token) noexcept
    {
        getInfoEvent.remove(token);
    }
}
