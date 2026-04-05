// SearchUserControl.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "SearchUserControl.xaml.h"
#if __has_include("SearchUserControl.g.cpp")
#include "SearchUserControl.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    SearchUserControl::SearchUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer)
    {
        object = CelestiaSelection();
    }

    void SearchUserControl::InitializeComponent()
    {
        SearchUserControlT::InitializeComponent();
    }

    void SearchUserControl::SearchBox_TextChanged(IInspectable const& sender, Controls::AutoSuggestBoxTextChangedEventArgs const& args)
    {
        auto suggestBox = sender.as<Controls::AutoSuggestBox>();
        auto text = suggestBox.Text();
        if (args.Reason() != Controls::AutoSuggestionBoxTextChangeReason::UserInput) return;

        if (text.empty())
        {
            object = CelestiaComponent::CelestiaSelection();
            suggestBox.ItemsSource(single_threaded_observable_vector<CelestiaComponent::CelestiaCompletion>());
            return;
        }

        renderer.EnqueueTask([weak_this{ get_weak() }, suggestBox, text]()
            {
                auto strong_this = weak_this.get();
                if (!strong_this) return;
                auto comCompletions = strong_this->appCore.Simulation().GetCompletion(text);
                auto fullMatch = strong_this->appCore.Simulation().Find(text);
                auto source = single_threaded_observable_vector<CelestiaComponent::CelestiaCompletion>();
                for (auto const& completion : comCompletions)
                    source.Append(completion);
                strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [text, source, suggestBox, fullMatch, strong_this]()
                    {
                        if (suggestBox.Text() == text)
                        {
                            strong_this->object = fullMatch;
                            suggestBox.ItemsSource(source);
                        }
                    });
            });
    }

    void SearchUserControl::SearchBox_SuggestionChosen(IInspectable const& sender, Controls::AutoSuggestBoxSuggestionChosenEventArgs const& args)
    {
        auto suggestBox = sender.as<Controls::AutoSuggestBox>();
        auto selected = args.SelectedItem().as<CelestiaComponent::CelestiaCompletion>();
        object = selected.Selection();
        suggestBox.Text(selected.Name());
    }

    void SearchUserControl::SearchBox_QuerySubmitted(Windows::Foundation::IInspectable const&, Controls::AutoSuggestBoxQuerySubmittedEventArgs const& args)
    {
        if (args.ChosenSuggestion() != nullptr)
        {
            auto selected = args.ChosenSuggestion().as<CelestiaComponent::CelestiaCompletion>();
            ShowSearchResult(selected.Selection());
        }
        else
        {
            ShowSearchResult(object);
        }
    }

    void SearchUserControl::ShowSearchResult(CelestiaComponent::CelestiaSelection const& selection)
    {
        SearchResultContainer().Children().Clear();
        if (selection.IsEmpty())
            return;

        InfoUserControl userControl{ appCore, renderer, selection, false };
        userControl.OpenURL([weak_this{ get_weak() }](IInspectable const&, hstring const& url)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->openURLEvent(*strong_this, url);
            });
        userControl.GetInfo([weak_this{ get_weak() }](IInspectable const&, CelestiaWinUI::InfoGetInfoArgs const& args)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->getInfoEvent(*strong_this, args);
            });
        userControl.ShowSubsystem([weak_this{ get_weak() }](IInspectable const&, CelestiaWinUI::InfoShowSubsystemArgs const& args)
            {
                auto strong_this{ weak_this.get() };
                if (strong_this == nullptr) return;
                strong_this->showSubsystemEvent(*strong_this, args);
            });
        SearchResultContainer().Children().Append(userControl);
        SearchResultContainer().SetAlignTopWithPanel(userControl, true);
        SearchResultContainer().SetAlignBottomWithPanel(userControl, true);
        SearchResultContainer().SetAlignLeftWithPanel(userControl, true);
        SearchResultContainer().SetAlignRightWithPanel(userControl, true);
    }

    event_token SearchUserControl::OpenURL(Windows::Foundation::EventHandler<hstring> const& handler)
    {
        return openURLEvent.add(handler);
    }

    void SearchUserControl::OpenURL(event_token const& token) noexcept
    {
        openURLEvent.remove(token);
    }

    event_token SearchUserControl::GetInfo(Windows::Foundation::EventHandler<CelestiaWinUI::InfoGetInfoArgs> const& handler)
    {
        return getInfoEvent.add(handler);
    }

    void SearchUserControl::GetInfo(event_token const& token) noexcept
    {
        getInfoEvent.remove(token);
    }

    event_token SearchUserControl::ShowSubsystem(Windows::Foundation::EventHandler<CelestiaWinUI::InfoShowSubsystemArgs> const& handler)
    {
        return showSubsystemEvent.add(handler);
    }

    void SearchUserControl::ShowSubsystem(event_token const& token) noexcept
    {
        showSubsystemEvent.remove(token);
    }
}