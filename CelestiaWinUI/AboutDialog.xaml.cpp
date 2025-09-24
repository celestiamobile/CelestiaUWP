// AboutDialog.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "AboutDialog.xaml.h"
#if __has_include("AboutDialog.g.cpp")
#include "AboutDialog.g.cpp"
#endif

#include <fmt/format.h>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    AboutDialog::AboutDialog(hstring const& authorsFilePath, hstring const& translatorsFilePath) : authorsFilePath(authorsFilePath), translatorsFilePath(translatorsFilePath)
    {
    }

    void AboutDialog::InitializeComponent()
    {
        AboutDialogT::InitializeComponent();

        Title(box_value(LocalizationHelper::Localize(L"About Celestia", L"System menu item")));
        PrimaryButtonText(LocalizationHelper::Localize(L"OK", L""));

        Windows::Web::Http::HttpFormUrlEncodedContent query({ { L"lang", LocalizationHelper::Locale()} });
        hstring url = hstring(L"https://celestia.mobi") + L"?" + query.ToString();
        LinkButton().NavigateUri(Uri(url));

        auto version = SystemInformation::Instance().ApplicationVersion();
        VersionLabel().Text(to_hstring(fmt::format("Celestia {0}.{1}.{2}.{3}", version.Major, version.Minor, version.Build, version.Revision)));

        LoadAuthorTranslator();
    }

    fire_and_forget AboutDialog::LoadAuthorTranslator()
    {
        try
        {
            auto strong_this{ get_strong() };
            auto authorFile{ co_await Windows::Storage::StorageFile::GetFileFromPathAsync(strong_this->authorsFilePath) };
            auto authorsText{ co_await Windows::Storage::FileIO::ReadTextAsync(authorFile) };
            auto translatorsFile{ co_await Windows::Storage::StorageFile::GetFileFromPathAsync(strong_this->translatorsFilePath) };
            auto translatorsText{ co_await Windows::Storage::FileIO::ReadTextAsync(translatorsFile) };
            strong_this->TranslatorLabel().Text(translatorsText);
            strong_this->TranslatorTitleLabel().Text(LocalizationHelper::Localize(L"Translators:", L"Translators for Celestia"));
            strong_this->AuthorLabel().Text(authorsText);
            strong_this->AuthorTitleLabel().Text(LocalizationHelper::Localize(L"Authors:", L"Authors for Celestia"));
        }
        catch (hresult_error const&) {}
    }
}
