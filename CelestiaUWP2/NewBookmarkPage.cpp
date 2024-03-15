//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "NewBookmarkPage.h"
#if __has_include("NewBookmarkParameter.g.cpp")
#include "NewBookmarkParameter.g.cpp"
#endif
#if __has_include("NewBookmarkPage.g.cpp")
#include "NewBookmarkPage.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::CelestiaUWP2::implementation
{
    NewBookmarkParameter::NewBookmarkParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer) {}
    CelestiaComponent::CelestiaAppCore NewBookmarkParameter::AppCore() { return appCore; }
    CelestiaComponent::CelestiaRenderer NewBookmarkParameter::Renderer() { return renderer;  }

    NewBookmarkPage::NewBookmarkPage() : appCore(nullptr), renderer(nullptr) {}

    void NewBookmarkPage::InitializeComponent()
    {
        NewBookmarkPageT::InitializeComponent();
        ConfirmButton().Content(box_value(LocalizationHelper::Localize(L"OK", L"")));
    }

    hstring NewBookmarkPage::NameText()
    {
        return nameText;
    }

    void NewBookmarkPage::NameText(hstring const& value)
    {
        nameText = value;
    }

    void NewBookmarkPage::ConfirmButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        if (nameText.empty()) return;
        auto name = nameText;

        renderer.EnqueueTask([this, name]()
            {
                auto url = appCore.CurrentURL();
                Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [this, name, url]()
                    {
                        BookmarkNode bookmark{ false, name, url, single_threaded_observable_vector<BookmarkNode>() };
                        if (auto organizerPage = Organizer().Content().try_as<CelestiaUWP2::BookmarkOrganizerPage>(); organizerPage != nullptr)
                            organizerPage.InsertBookmarkAtSelection(bookmark);
                    });
            });
    }

    void NewBookmarkPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto parameter = e.Parameter().as<CelestiaUWP2::NewBookmarkParameter>();
        appCore = parameter.AppCore();
        renderer = parameter.Renderer();
        Organizer().Navigate(xaml_typename<CelestiaUWP2::BookmarkOrganizerPage>(), CelestiaUWP2::BookmarkOrganizerParameter(appCore, renderer, true));
    }
}
