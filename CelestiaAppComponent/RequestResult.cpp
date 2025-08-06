// RequestResult.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "RequestResult.h"
#if __has_include("RequestResultInfo.g.cpp")
#include "RequestResultInfo.g.cpp"
#endif
#if __has_include("RequestResult.g.cpp")
#include "RequestResult.g.cpp"
#endif

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Data::Json;

namespace winrt::CelestiaAppComponent::implementation
{
    RequestResultInfo::RequestResultInfo(hstring const& detail, hstring const& reason) : detail(detail), reason(reason)
    {
    }

    hstring RequestResultInfo::Reason()
    {
        return reason;
    }

    hstring RequestResultInfo::Detail()
    {
        return detail;
    }

    RequestResult::RequestResult(CelestiaAppComponent::RequestResultInfo const& info, int32_t status) : info(info), status(status)
    {
    }

    CelestiaAppComponent::RequestResultInfo RequestResult::Info()
    {
        return info;
    }

    int32_t RequestResult::Status()
    {
        return status;
    }

    CelestiaAppComponent::RequestResult RequestResult::TryParse(hstring const& data)
    {
        auto object = JsonObject::Parse(data);
        // Get mandatory fields
        auto status = static_cast<int32_t>(object.GetNamedNumber(L"status"));
        auto infoObject = object.GetNamedObject(L"info");
        // Get optional fields
        auto detail = infoObject.GetNamedString(L"detail", L"");
        auto reason = infoObject.GetNamedString(L"reason", L"");
        return CelestiaAppComponent::RequestResult(CelestiaAppComponent::RequestResultInfo(detail, reason), status);
    }
}
