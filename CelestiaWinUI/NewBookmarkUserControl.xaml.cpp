#include "pch.h"
#include "NewBookmarkUserControl.xaml.h"
#if __has_include("NewBookmarkUserControl.g.cpp")
#include "NewBookmarkUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    NewBookmarkUserControl::NewBookmarkUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer), organizer(appCore, renderer, true)
    {
        InitializeComponent();
        ConfirmButton().Content(box_value(LocalizationHelper::Localize(L"OK")));
        OrganizerContainer().Children().Append(organizer);
        OrganizerContainer().SetAlignTopWithPanel(organizer, true);
        OrganizerContainer().SetAlignBottomWithPanel(organizer, true);
        OrganizerContainer().SetAlignLeftWithPanel(organizer, true);
        OrganizerContainer().SetAlignRightWithPanel(organizer, true);
    }

    hstring NewBookmarkUserControl::NameText()
    {
        return nameText;
    }

    void NewBookmarkUserControl::NameText(hstring const& value)
    {
        nameText = value;
    }

    void NewBookmarkUserControl::ConfirmButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (nameText.empty()) return;
        auto name = nameText;

        renderer.EnqueueTask([this, name]()
            {
                auto url = appCore.CurrentURL();
                DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [this, name, url]()
                    {
                        BookmarkNode bookmark{ false, name, url, single_threaded_observable_vector<BookmarkNode>() };
                        organizer.InsertBookmarkAtSelection(bookmark);
                    });
            });
    }
}