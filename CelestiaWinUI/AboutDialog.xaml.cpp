// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#include "pch.h"
#include "AboutDialog.xaml.h"
#if __has_include("AboutDialog.g.cpp")
#include "AboutDialog.g.cpp"
#endif

#include <fmt/format.h>
#include <fmt/xchar.h>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    AboutDialog::AboutDialog(hstring const& authorsFilePath, hstring const& translatorsFilePath)
    {
        InitializeComponent();

        Title(box_value(LocalizationHelper::Localize(L"About Celestia")));
        PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        auto version = SystemInformation::Instance().ApplicationVersion();

        VersionLabel().Text(hstring(fmt::format(L"Celestia {0}.{1}.{2}.{3}", version.Major, version.Minor, version.Build, version.Revision)));

        LoadAuthorTranslator(authorsFilePath, translatorsFilePath);
    }

    fire_and_forget AboutDialog::LoadAuthorTranslator(hstring const authorsFilePath, hstring const translatorsFilePath)
    {
        try
        {
            auto authorFile{ co_await Windows::Storage::StorageFile::GetFileFromPathAsync(authorsFilePath) };
            auto authorsText = co_await Windows::Storage::FileIO::ReadTextAsync(authorFile);
            AuthorLabel().Text(authorsText);
            AuthorTitleLabel().Text(LocalizationHelper::Localize(L"Authors:"));
            auto translatorsFile{ co_await Windows::Storage::StorageFile::GetFileFromPathAsync(translatorsFilePath) };
            auto translatorsText = co_await Windows::Storage::FileIO::ReadTextAsync(translatorsFile);
            TranslatorLabel().Text(translatorsText);
            TranslatorTitleLabel().Text(LocalizationHelper::Localize(L"Translators:"));
        }
        catch (hresult_error const&) {}
    }
}
