//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "AboutDialog.h"
#if __has_include("AboutDialog.g.cpp")
#include "AboutDialog.g.cpp"
#endif

#include <fmt/format.h>
#include <fmt/xchar.h>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::CelestiaUWP2::implementation
{
    AboutDialog::AboutDialog(hstring const& authorsFilePath, hstring const& translatorsFilePath) : authorsFilePath(authorsFilePath), translatorsFilePath(translatorsFilePath)
    {
    }

    void AboutDialog::InitializeComponent()
    {
        AboutDialogT::InitializeComponent();

        Title(box_value(LocalizationHelper::Localize(L"About Celestia", L"System menu item")));
        PrimaryButtonText(LocalizationHelper::Localize(L"OK", L""));
        auto version = SystemInformation::Instance().ApplicationVersion();

        VersionLabel().Text(hstring(fmt::format(L"Celestia {0}.{1}.{2}.{3}", version.Major, version.Minor, version.Build, version.Revision)));

        LoadAuthorTranslator();
    }

    fire_and_forget AboutDialog::LoadAuthorTranslator()
    {
        try
        {
            auto authorFile{ co_await Windows::Storage::StorageFile::GetFileFromPathAsync(authorsFilePath) };
            auto authorsText = co_await Windows::Storage::FileIO::ReadTextAsync(authorFile);
            AuthorLabel().Text(authorsText);
            AuthorTitleLabel().Text(LocalizationHelper::Localize(L"Authors:", L"Authors for Celestia"));
            auto translatorsFile{ co_await Windows::Storage::StorageFile::GetFileFromPathAsync(translatorsFilePath) };
            auto translatorsText = co_await Windows::Storage::FileIO::ReadTextAsync(translatorsFile);
            TranslatorLabel().Text(translatorsText);
            TranslatorTitleLabel().Text(LocalizationHelper::Localize(L"Translators:", L"Translators for Celestia"));
        }
        catch (hresult_error const&) {}
    }
}
