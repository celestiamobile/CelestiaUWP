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
            return box_value(LocalizationHelper::Localize(L"Free Flight"));
        case CoordinateSystem::Ecliptical:
            return box_value(LocalizationHelper::Localize(L"Follow"));
        case CoordinateSystem::BodyFixed:
            return box_value(LocalizationHelper::Localize(L"Sync Orbit"));
        case CoordinateSystem::PhaseLock:
            return box_value(LocalizationHelper::Localize(L"Phase Lock"));
        case CoordinateSystem::Chase:
            return box_value(LocalizationHelper::Localize(L"Chase"));
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
        InitializeComponent();
        CoordinateSystemHint().Text(LocalizationHelper::Localize(L"Coordinate System:"));
        ReferenceNameText().PlaceholderText(LocalizationHelper::Localize(L"Reference Object Name"));
        TargetNameText().PlaceholderText(LocalizationHelper::Localize(L"Target Object Name"));

        auto infoText = static_cast<std::wstring>(LocalizationHelper::Localize(L"Flight mode decides how you move around in Celestia. Learn more\u2026"));
        auto linkInfoText = static_cast<std::wstring>(LocalizationHelper::Localize(L"Learn more\u2026"));
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
        PrimaryButtonText(LocalizationHelper::Localize(L"OK"));
        SecondaryButtonText(LocalizationHelper::Localize(L"Cancel"));
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

    hstring ObserverModeDialog::ReferenceObjectName()
    {
        return RefObjectTextBoxVisibility() == Visibility::Visible ? ReferenceNameText().Text() : L"";
    }

    hstring ObserverModeDialog::TargetObjectName()
    {
        return TargetObjectTextBoxVisibility() == Visibility::Visible ? TargetNameText().Text() : L"";
    }

    void ObserverModeDialog::ObjectNameText_TextChanged(IInspectable const& sender, Controls::AutoSuggestBoxTextChangedEventArgs const& args)
    {
        if (args.Reason() != Controls::AutoSuggestionBoxTextChangeReason::UserInput) return;

        auto suggestBox = sender.as<Controls::AutoSuggestBox>();
        auto text = suggestBox.Text();
        if (text.empty())
        {
            suggestBox.ItemsSource(single_threaded_observable_vector<hstring>());
            return;
        }

        renderer.EnqueueTask([weak_this{ get_weak() }, suggestBox, text]()
            {
                auto strong_this = weak_this.get();
                if (!strong_this) return;
                auto comCompletions = strong_this->appCore.Simulation().GetCompletion(text);
                std::vector<hstring> completions{ comCompletions.begin(), comCompletions.end() };
                strong_this->DispatcherQueue().TryEnqueue(Microsoft::UI::Dispatching::DispatcherQueuePriority::Normal, [text, completions, suggestBox]()
                    {
                        if (suggestBox.Text() == text)
                        {
                            auto source = single_threaded_observable_vector<hstring>();
                            source.ReplaceAll(completions);
                            suggestBox.ItemsSource(source);
                        }
                    });
            });
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