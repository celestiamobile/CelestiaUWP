// EmailHelper.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

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
