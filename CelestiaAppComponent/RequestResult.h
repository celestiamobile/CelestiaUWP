#pragma once

#include "RequestResultInfo.g.h"
#include "RequestResult.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct RequestResultInfo : RequestResultInfoT<RequestResultInfo>
    {
        RequestResultInfo(hstring const& detail, hstring const& reason);
        hstring Detail();
        hstring Reason();

    private:
        hstring detail;
        hstring reason;
    };

    struct RequestResult : RequestResultT<RequestResult>
    {
        RequestResult(CelestiaAppComponent::RequestResultInfo const& info, int32_t status);
        CelestiaAppComponent::RequestResultInfo Info();
        int32_t Status();

        static CelestiaAppComponent::RequestResult TryParse(hstring const& data);

    private:
        CelestiaAppComponent::RequestResultInfo info;
        int32_t status;
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct RequestResultInfo : RequestResultInfoT<RequestResultInfo, implementation::RequestResultInfo>
    {
    };

    struct RequestResult : RequestResultT<RequestResult, implementation::RequestResult>
    {
    };
}
