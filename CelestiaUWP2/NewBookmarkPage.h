//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "NewBookmarkParameter.g.h"
#include "NewBookmarkPage.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct NewBookmarkParameter : NewBookmarkParameterT<NewBookmarkParameter>
    {
        NewBookmarkParameter(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
    };

    struct NewBookmarkPage : NewBookmarkPageT<NewBookmarkPage>
    {
        NewBookmarkPage();
        void InitializeComponent();

        hstring NameText();
        void NameText(hstring const&);

        void ConfirmButton_Click(Windows::Foundation::IInspectable const&, Windows::UI::Xaml::RoutedEventArgs const&);
        void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs const& e);

    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        hstring nameText{ L"" };
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct NewBookmarkParameter : NewBookmarkParameterT<NewBookmarkParameter, implementation::NewBookmarkParameter>
    {
    };

    struct NewBookmarkPage : NewBookmarkPageT<NewBookmarkPage, implementation::NewBookmarkPage>
    {
    };
}
