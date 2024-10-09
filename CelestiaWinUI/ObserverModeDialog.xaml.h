// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

#pragma once

#include "ObserverModeStringConverter.g.h"
#include "ObserverModeDialog.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct ObserverModeStringConverter : ObserverModeStringConverterT<ObserverModeStringConverter>
    {
        ObserverModeStringConverter() = default;
        Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
        Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    };

    struct ObserverModeDialog : ObserverModeDialogT<ObserverModeDialog>
    {
        ObserverModeDialog(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaRenderer const& renderer);
        void InitializeComponent();

        Windows::Foundation::Collections::IObservableVector<CelestiaComponent::CoordinateSystem> CoordinateSystems();
        CelestiaComponent::CoordinateSystem SelectedCoordinateSystem();
        void SelectedCoordinateSystem(CelestiaComponent::CoordinateSystem);
        Microsoft::UI::Xaml::Visibility RefObjectTextBoxVisibility();
        Microsoft::UI::Xaml::Visibility TargetObjectTextBoxVisibility();
        CelestiaComponent::CelestiaSelection ReferenceObject();
        CelestiaComponent::CelestiaSelection TargetObject();

        void ObjectNameText_TextChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxTextChangedEventArgs const&);
        void ObjectNameText_SuggestionChosen(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::AutoSuggestBoxSuggestionChosenEventArgs const&);

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;
    private:
        CelestiaComponent::CelestiaAppCore appCore;
        CelestiaComponent::CelestiaRenderer renderer;
        CelestiaComponent::CelestiaSelection referenceObject;
        CelestiaComponent::CelestiaSelection targetObject;
        Windows::Foundation::Collections::IObservableVector<CelestiaComponent::CoordinateSystem> coordinateSystems;
        CelestiaComponent::CoordinateSystem selectedCoordinateSystem{ CelestiaComponent::CoordinateSystem::Universal };
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChangedEvent;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct ObserverModeStringConverter : ObserverModeStringConverterT<ObserverModeStringConverter, implementation::ObserverModeStringConverter>
    {
    };

    struct ObserverModeDialog : ObserverModeDialogT<ObserverModeDialog, implementation::ObserverModeDialog>
    {
    };
}
