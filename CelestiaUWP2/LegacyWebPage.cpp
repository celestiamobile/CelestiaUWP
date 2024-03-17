//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "LegacyWebPage.h"
#if __has_include("LegacyWebPage.g.cpp")
#include "LegacyWebPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::CelestiaUWP2::implementation
{
    LegacyWebPage::LegacyWebPage() : initialUri(nullptr), matchingQueryKeys(nullptr)
    {
    }

    fire_and_forget LegacyWebPage::WebView_NavigationStarting(IInspectable const&, WebViewNavigationStartingEventArgs const& args)
    {
        if (args.Uri() == nullptr)
        {
            args.Cancel(true);
            return;
        }
        Uri uri = args.Uri();
        if (!IsURIAllowed(uri))
        {
            args.Cancel(true);
            co_await Windows::System::Launcher::LaunchUriAsync(uri);
        }
    }

    void LegacyWebPage::WebView_ScriptNotify(IInspectable const&, NotifyEventArgs const& args)
    {
        try
        {
            auto message = args.Value();
            if (!message.empty() && bridge != nullptr)
            {
                bridge.ReceivedMessage(message);
            }
        }
        catch (hresult_error const&) {}
    }

    void LegacyWebPage::WebView_DOMContentLoaded(IInspectable const&, WebViewDOMContentLoadedEventArgs const&)
    {
        LoadingIndicator().Visibility(Visibility::Collapsed);
    }

    bool LegacyWebPage::IsURIAllowed(Uri const& uri)
    {
        if (uri.Host() != initialUri.Host() || uri.Path() != initialUri.Path())
            return false;

        auto query = uri.QueryParsed();
        auto initialQuery = initialUri.QueryParsed();

        for (const auto& key : matchingQueryKeys)
        {
            if (query.GetFirstValueByName(key) != initialQuery.GetFirstValueByName(key))
                return false;
        }
        return true;
    }

    void LegacyWebPage::OnNavigatedTo(Navigation::NavigationEventArgs const& e)
    {
        auto parameter = e.Parameter().as<CelestiaUWP2::CommonWebParameter>();
        auto uri = parameter.InitialUri();
        if (uri.Query().empty())
            initialUri = uri.CombineUri(ActualTheme() == Windows::UI::Xaml::ElementTheme::Dark ? L"theme=dark" : L"theme=light");
        else
            initialUri = uri.CombineUri(ActualTheme() == Windows::UI::Xaml::ElementTheme::Dark ? L"&theme=dark" : L"&theme=light");
        matchingQueryKeys = parameter.MatchingQueryKeys();
        bridge = CelestiaAppComponent::JavascriptBridge(parameter.AppCore(), parameter.Renderer(), parameter.ContextDirectory(), parameter.ACKCallback(), [weak_this{ get_weak() }](hstring const title, Uri const uri)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->Dispatcher().TryRunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, [title, uri]()
                    {
                        using namespace Windows::ApplicationModel::DataTransfer;
                        auto transferManager = DataTransferManager::GetForCurrentView();
                        transferManager.DataRequested([title, uri](IInspectable const&, DataRequestedEventArgs const& args)
                            {
                                args.Request().Data().Properties().Title(title);
                                args.Request().Data().SetWebLink(uri);
                            });
                        DataTransferManager::ShowShareUI();
                    });
            });

        WebView().Source(initialUri);
        WebView().DOMContentLoaded({ get_weak(), &LegacyWebPage::WebView_DOMContentLoaded });
    }
}
