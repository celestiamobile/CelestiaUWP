#pragma once

#include "EmailHelper.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct EmailHelper : EmailHelperT<EmailHelper>
    {
        static Windows::Foundation::Uri UriForEmail(hstring const& recipient, hstring const& subject, hstring const& body, Windows::Foundation::Collections::IVector<Windows::Storage::StorageFile> const& attachments);
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct EmailHelper : EmailHelperT<EmailHelper, implementation::EmailHelper>
    {
    };
}
