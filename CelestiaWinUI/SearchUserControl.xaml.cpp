// SearchUserControl.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include "SearchUserControl.xaml.h"
#if __has_include("SearchUserControl.g.cpp")
#include "SearchUserControl.g.cpp"
#endif
#include <algorithm>
#include <utility>

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;
using namespace Windows::Data::Json;

namespace winrt::CelestiaWinUI::implementation
{
    SearchUserControl::SearchUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer)
    {
    }

    void SearchUserControl::InitializeComponent()
    {
        SearchUserControlT::InitializeComponent();
        RecentSearchTitle().Text(LocalizationHelper::Localize(L"Recent Searches", L""));
        const auto clearHistoryTitle = LocalizationHelper::Localize(L"Clear History", L"");
        Automation::AutomationProperties::SetName(ClearHistoryButton(), clearHistoryTitle);
        ToolTipService::SetToolTip(ClearHistoryButton(), box_value(clearHistoryTitle));
        SearchBox().Loaded([](IInspectable const& sender, RoutedEventArgs const&)
            {
                sender.as<AutoSuggestBox>().Focus(FocusState::Programmatic);
            });
        UpdateRecentSearches();
    }

    void SearchUserControl::FocusSearchBox()
    {
        SearchBox().Focus(FocusState::Programmatic);
    }

    std::vector<hstring> SearchUserControl::ReadRecentSearches()
    {
        std::vector<hstring> paths;
        auto stored = AppDataHelper::LocalSettings().Values().TryLookup(L"RecentSearches");
        if (!stored) return paths;
        try
        {
            auto values = JsonArray::Parse(unbox_value<hstring>(stored));
            for (auto const& value : values)
            {
                auto path = value.GetString();
                if (!path.empty() && std::find(paths.begin(), paths.end(), path) == paths.end())
                    paths.push_back(path);
                if (paths.size() == 10) break;
            }
        }
        catch (hresult_error const& error)
        {
            CelestiaLogger::Log(hstring{ L"Unable to read recent searches: " } + error.message());
            paths.clear();
        }
        return paths;
    }

    void SearchUserControl::UpdateRecentSearches()
    {
        const auto generation = ++recentSearchGeneration;
        RecentSearchList().Items().Clear();
        RecentSearchContainer().Visibility(Visibility::Collapsed);
        SearchResultContainer().Visibility(Visibility::Visible);
        if (!SearchBox().Text().empty()) return;
        auto paths = ReadRecentSearches();
        if (paths.empty()) return;
        renderer.EnqueueTask([weak_this{ get_weak() }, paths = std::move(paths), generation]()
            {
                auto strong_this = weak_this.get();
                if (!strong_this) return;
                std::vector<std::pair<hstring, hstring>> entries;
                auto simulation = strong_this->appCore.Simulation();
                for (auto const& path : paths)
                {
                    auto selection = simulation.Find(path);
                    if (!selection.IsEmpty())
                        entries.emplace_back(path, simulation.Universe().NameForSelection(selection));
                }
                strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [strong_this, entries = std::move(entries), generation]()
                    {
                        if (strong_this->recentSearchGeneration != generation) return;
                        for (auto const& [path, name] : entries)
                        {
                            TextBlock item;
                            item.Text(name);
                            item.Tag(box_value(path));
                            strong_this->RecentSearchList().Items().Append(item);
                        }
                        bool visible = !entries.empty();
                        strong_this->RecentSearchContainer().Visibility(visible ? Visibility::Visible : Visibility::Collapsed);
                        strong_this->SearchResultContainer().Visibility(visible ? Visibility::Collapsed : Visibility::Visible);
                    });
            });
    }

    void SearchUserControl::RecordRecentSearch(hstring const& path)
    {
        if (path.empty())
        {
            CelestiaLogger::Log(L"Selected object has no search path");
            return;
        }
        auto paths = ReadRecentSearches();
        paths.erase(std::remove(paths.begin(), paths.end(), path), paths.end());
        paths.insert(paths.begin(), path);
        if (paths.size() > 10) paths.resize(10);
        JsonArray values;
        for (auto const& entry : paths)
            values.Append(JsonValue::CreateStringValue(entry));
        try
        {
            AppDataHelper::LocalSettings().Values().Insert(L"RecentSearches", box_value(values.Stringify()));
        }
        catch (hresult_error const& error)
        {
            CelestiaLogger::Log(hstring{ L"Unable to save recent searches: " } + error.message());
        }
    }

    void SearchUserControl::ClearHistoryButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        ++selectionGeneration;
        try
        {
            auto values = AppDataHelper::LocalSettings().Values();
            if (values.HasKey(L"RecentSearches"))
                values.Remove(L"RecentSearches");
        }
        catch (hresult_error const& error)
        {
            CelestiaLogger::Log(hstring{ L"Unable to clear recent searches: " } + error.message());
        }
        UpdateRecentSearches();
    }

    void SearchUserControl::RecentSearchList_ItemClick(IInspectable const&, ItemClickEventArgs const& args)
    {
        auto item = args.ClickedItem().as<TextBlock>();
        auto path = unbox_value<hstring>(item.Tag());
        SearchBox().Text(item.Text());
        ShowSearchResult(nullptr, path);
    }

    void SearchUserControl::SearchBox_TextChanged(IInspectable const& sender, Controls::AutoSuggestBoxTextChangedEventArgs const& args)
    {
        auto suggestBox = sender.as<Controls::AutoSuggestBox>();
        auto text = suggestBox.Text();
        if (args.Reason() != Controls::AutoSuggestionBoxTextChangeReason::UserInput) return;
        retainedSelection = nullptr;
        retainedQuery = L"";
        const auto generation = ++searchGeneration;
        ++selectionGeneration;
        UpdateRecentSearches();

        if (text.empty())
        {
            suggestBox.ItemsSource(single_threaded_observable_vector<CelestiaComponent::CelestiaCompletion>());
            return;
        }

        renderer.EnqueueTask([weak_this{ get_weak() }, suggestBox, text, generation]()
            {
                auto strong_this = weak_this.get();
                if (!strong_this) return;
                auto comCompletions = strong_this->appCore.Simulation().GetCompletion(text);
                auto source = single_threaded_observable_vector<CelestiaComponent::CelestiaCompletion>();
                for (auto const& completion : comCompletions)
                    source.Append(completion);
                strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [text, source, suggestBox, strong_this, generation]()
                    {
                        if (strong_this->searchGeneration == generation && suggestBox.Text() == text)
                        {
                            suggestBox.ItemsSource(source);
                        }
                    });
            });
    }

    void SearchUserControl::SearchBox_SuggestionChosen(IInspectable const& sender, Controls::AutoSuggestBoxSuggestionChosenEventArgs const& args)
    {
        auto suggestBox = sender.as<Controls::AutoSuggestBox>();
        auto selected = args.SelectedItem().as<CelestiaComponent::CelestiaCompletion>();
        ++selectionGeneration;
        retainedSelection = selected.Selection();
        retainedQuery = L"";
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
            if (!SearchBox().Text().empty())
                ShowSearchResult(retainedSelection, retainedQuery.empty() ? SearchBox().Text() : retainedQuery);
        }
    }

    void SearchUserControl::ShowSearchResult(CelestiaComponent::CelestiaSelection const& selection, hstring const& query)
    {
        retainedSelection = selection;
        retainedQuery = query;
        const auto generation = ++selectionGeneration;
        ++recentSearchGeneration;
        renderer.EnqueueTask([weak_this{ get_weak() }, selection, query, generation]()
            {
                auto strong_this = weak_this.get();
                if (!strong_this) return;
                auto resolved = selection ? selection : strong_this->appCore.Simulation().Find(query);
                auto path = strong_this->appCore.Simulation().Universe().PathForSelection(resolved);
                strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [strong_this, resolved, path, generation]()
                    {
                        if (strong_this->selectionGeneration != generation) return;
                        if (!resolved.IsEmpty())
                        {
                            strong_this->retainedSelection = resolved;
                            strong_this->RecordRecentSearch(path);
                        }
                        else
                        {
                            strong_this->retainedSelection = nullptr;
                        }
                        strong_this->PresentSearchResult(resolved);
                    });
            });
    }

    void SearchUserControl::PresentSearchResult(CelestiaComponent::CelestiaSelection const& selection)
    {
        RecentSearchContainer().Visibility(Visibility::Collapsed);
        SearchResultContainer().Visibility(Visibility::Visible);
        SearchResultContainer().Children().Clear();
        if (selection.IsEmpty())
        {
            TextBlock error;
            error.Text(LocalizationHelper::Localize(L"Object not found", L""));
            SearchResultContainer().Children().Append(error);
            return;
        }

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