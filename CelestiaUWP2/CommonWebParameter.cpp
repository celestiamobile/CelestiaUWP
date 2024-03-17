//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"

#include "CommonWebParameter.h"
#if __has_include("CommonWebParameter.g.cpp")
#include "CommonWebParameter.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::CelestiaUWP2::implementation
{
    CommonWebParameter::CommonWebParameter(Uri const& initialUri, Collections::IVector<hstring> const& matchingQueryKeys, CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer, hstring const& contextDirectory, CelestiaAppComponent::JavascriptMessageACKCallback const& ackCallback)
        : initialUri(initialUri), matchingQueryKeys(matchingQueryKeys), appCore(appCore), renderer(renderer), contextDirectory(contextDirectory), ackCallback(ackCallback)
    {
    }

    Uri CommonWebParameter::InitialUri()
    {
        return initialUri;
    }

    Collections::IVector<hstring> CommonWebParameter::MatchingQueryKeys()
    {
        return matchingQueryKeys;
    }

    CelestiaComponent::CelestiaAppCore CommonWebParameter::AppCore() {
        return appCore;
    }

    CelestiaComponent::CelestiaRenderer CommonWebParameter::Renderer()
    {
        return renderer;
    }

    hstring CommonWebParameter::ContextDirectory()
    {
        return contextDirectory;
    }

    CelestiaAppComponent::JavascriptMessageACKCallback CommonWebParameter::ACKCallback()
    {
        return ackCallback;
    };
}