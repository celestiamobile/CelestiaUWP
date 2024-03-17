//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "CommonWebParameter.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct CommonWebParameter : CommonWebParameterT<CommonWebParameter>
    {
        CommonWebParameter(Windows::Foundation::Uri const& initialUri, Windows::Foundation::Collections::IVector<hstring> const& matchingQueryKeys, CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, hstring const& contextDirectory, CelestiaAppComponent::JavascriptMessageACKCallback const& ackCallback);

        Windows::Foundation::Uri InitialUri();
        Windows::Foundation::Collections::IVector<hstring> MatchingQueryKeys();
        CelestiaComponent::CelestiaAppCore AppCore();
        CelestiaComponent::CelestiaRenderer Renderer();
        hstring ContextDirectory();
        CelestiaAppComponent::JavascriptMessageACKCallback ACKCallback();

    private:
        Windows::Foundation::Uri initialUri;
        Windows::Foundation::Collections::IVector<hstring> matchingQueryKeys;
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        hstring contextDirectory;
        CelestiaAppComponent::JavascriptMessageACKCallback ackCallback;
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct CommonWebParameter : CommonWebParameterT<CommonWebParameter, implementation::CommonWebParameter>
    {
    };
}