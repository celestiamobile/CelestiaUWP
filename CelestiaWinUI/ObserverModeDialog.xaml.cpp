// ObserverModeDialog.xaml.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "ObserverModeDialog.xaml.h"
#if __has_include("ObserverModeStringConverter.g.cpp")
#include "ObserverModeStringConverter.g.cpp"
#endif
#if __has_include("ObserverModeDialog.g.cpp")
#include "ObserverModeDialog.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Documents;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    IInspectable ObserverModeStringConverter::Convert(IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const&, IInspectable const&, hstring const&)
    {
        auto coordinateSystem = unbox_value_or<CoordinateSystem>(value, CoordinateSystem::Universal);
        switch (coordinateSystem)
        {
        case CoordinateSystem::Universal:
            return box_value(LocalizationHelper::Localize(L"Free Flight", L"Flight mode, coordinate system"));
        case CoordinateSystem::Ecliptical:
            return box_value(LocalizationHelper::Localize(L"Follow", L""));
        case CoordinateSystem::BodyFixed:
            return box_value(LocalizationHelper::Localize(L"Sync Orbit", L""));
        case CoordinateSystem::PhaseLock:
            return box_value(LocalizationHelper::Localize(L"Phase Lock", L"Flight mode, coordinate system"));
        case CoordinateSystem::Chase:
            return box_value(LocalizationHelper::Localize(L"Chase", L""));
        default:
            break;
        }
        return nullptr;
    }

    IInspectable ObserverModeStringConverter::ConvertBack(IInspectable const&, Windows::UI::Xaml::Interop::TypeName const&, IInspectable const&, hstring const&)
    {
        return nullptr;
    }

    ObserverModeDialog::ObserverModeDialog(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer) : appCore(appCore), renderer(renderer)
    {
        coordinateSystems = single_threaded_observable_vector<CoordinateSystem>({ CoordinateSystem::Universal, CoordinateSystem::Ecliptical, CoordinateSystem::BodyFixed, CoordinateSystem::PhaseLock, CoordinateSystem::Chase });
        targetObject = CelestiaComponent::CelestiaSelection();
        referenceObject = CelestiaComponent::CelestiaSelection();
    }

    void ObserverModeDialog::InitializeComponent()
    {
        ObserverModeDialogT::InitializeComponent();
        CoordinateSystemHint().Text(LocalizationHelper::Localize(L"Coordinate System:", L"Used in Flight Mode"));
        ReferenceNameText().PlaceholderText(LocalizationHelper::Localize(L"Reference Object", L"Used in Flight Mode"));
        TargetNameText().PlaceholderText(LocalizationHelper::Localize(L"Target Object", L"Used in Flight Mode"));

        auto infoText = static_cast<std::wstring>(LocalizationHelper::Localize(L"Flight mode decides how you move around in Celestia. Learn more\u2026", L""));
        auto linkInfoText = static_cast<std::wstring>(LocalizationHelper::Localize(L"Learn more\u2026", L"Text for the link in Flight mode decides how you move around in Celestia. Learn more\u2026"));
        auto pos = infoText.find(linkInfoText);
        
        if (pos != std::wstring::npos)
        {
            Run textBefore;
            textBefore.Text(infoText.substr(0, pos));
            Hyperlink link;
            Run linkRun;
            linkRun.Text(linkInfoText);
            link.Inlines().Append(linkRun);

            Windows::Web::Http::HttpFormUrlEncodedContent query({ { L"lang", LocalizationHelper::Locale()} });
            hstring url = hstring(L"https://celestia.mobi/help/flight-mode") + L"?" + query.ToString();
            link.NavigateUri(Uri(url));

            Run textAfter;
            textAfter.Text(infoText.substr(pos + linkInfoText.size()));
            LearnMoreHint().Inlines().Append(textBefore);
            LearnMoreHint().Inlines().Append(link);
            LearnMoreHint().Inlines().Append(textAfter);
        }
        else
        {
            LearnMoreHint().Visibility(Visibility::Collapsed);
        }
        PrimaryButtonText(LocalizationHelper::Localize(L"OK", L""));
        SecondaryButtonText(LocalizationHelper::Localize(L"Cancel", L""));
    }

    Collections::IObservableVector<CoordinateSystem> ObserverModeDialog::CoordinateSystems()
    {
        return coordinateSystems;
    }

    CoordinateSystem ObserverModeDialog::SelectedCoordinateSystem()
    {
        return selectedCoordinateSystem;
    }

    void ObserverModeDialog::SelectedCoordinateSystem(CoordinateSystem value)
    {
        selectedCoordinateSystem = value;
        propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"RefObjectTextBoxVisibility"));
        propertyChangedEvent(*this, Data::PropertyChangedEventArgs(L"TargetObjectTextBoxVisibility"));
    }

    Visibility ObserverModeDialog::RefObjectTextBoxVisibility()
    {
        return selectedCoordinateSystem == CoordinateSystem::Universal ? Visibility::Collapsed : Visibility::Visible;
    }

    Visibility ObserverModeDialog::TargetObjectTextBoxVisibility()
    {
        return selectedCoordinateSystem == CoordinateSystem::PhaseLock ? Visibility::Visible : Visibility::Collapsed;
    }

    CelestiaComponent::CelestiaSelection ObserverModeDialog::ReferenceObject()
    {
        return RefObjectTextBoxVisibility() == Visibility::Visible ? referenceObject : CelestiaComponent::CelestiaSelection();
    }

    CelestiaComponent::CelestiaSelection ObserverModeDialog::TargetObject()
    {
        return TargetObjectTextBoxVisibility() == Visibility::Visible ? targetObject : CelestiaComponent::CelestiaSelection();
    }

    void ObserverModeDialog::ObjectNameText_TextChanged(IInspectable const& sender, Controls::AutoSuggestBoxTextChangedEventArgs const& args)
    {
        auto suggestBox = sender.as<Controls::AutoSuggestBox>();
        auto text = suggestBox.Text();
        bool isReference = suggestBox == ReferenceNameText();
        if (args.Reason() != Controls::AutoSuggestionBoxTextChangeReason::UserInput) return;

        if (text.empty())
        {
            suggestBox.ItemsSource(single_threaded_observable_vector<CelestiaComponent::CelestiaCompletion>());
            if (isReference)
                referenceObject = CelestiaComponent::CelestiaSelection();
            else
                targetObject = CelestiaComponent::CelestiaSelection();
            return;
        }

        renderer.EnqueueTask([weak_this{ get_weak() }, suggestBox, text, isReference]()
            {
                auto strong_this = weak_this.get();
                if (!strong_this) return;
                auto comCompletions = strong_this->appCore.Simulation().GetCompletion(text);
                auto fullMatch = strong_this->appCore.Simulation().Find(text);
                auto source = single_threaded_observable_vector<CelestiaComponent::CelestiaCompletion>();
                for (auto const& completion : comCompletions)
                    source.Append(completion);
                strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [text, source, suggestBox, strong_this, isReference, fullMatch]()
                    {
                        if (suggestBox.Text() == text)
                        {
                            suggestBox.ItemsSource(source);
                            if (isReference)
                                strong_this->referenceObject = fullMatch;
                            else
                                strong_this->targetObject = fullMatch;
                        }
                    });
            });
    }

    void ObserverModeDialog::ObjectNameText_SuggestionChosen(IInspectable const& sender, Controls::AutoSuggestBoxSuggestionChosenEventArgs const& args)
    {
        auto suggestBox = sender.as<Controls::AutoSuggestBox>();
        bool isReference = suggestBox == ReferenceNameText();
        auto selected = args.SelectedItem().as<CelestiaComponent::CelestiaCompletion>();
        if (isReference)
            referenceObject = selected.Selection();
        else
            targetObject = selected.Selection();
        suggestBox.Text(selected.Name());
    }


    event_token ObserverModeDialog::PropertyChanged(Data::PropertyChangedEventHandler const& handler)
    {
        return propertyChangedEvent.add(handler);
    }

    void ObserverModeDialog::PropertyChanged(event_token const& token) noexcept
    {
        propertyChangedEvent.remove(token);
    }
}
