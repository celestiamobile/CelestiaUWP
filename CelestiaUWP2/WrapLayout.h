//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "WrapItem.g.h"
#include "WrapLayoutState.g.h"
#include "WrapLayout.g.h"

namespace winrt::CelestiaUWP2::implementation
{
    struct WrapItem : WrapItemT<WrapItem>
    {
        WrapItem(int32_t index);

        int32_t Index();
        Windows::Foundation::IReference<CelestiaUWP2::UvMeasure> Measure();
        Windows::Foundation::IReference<CelestiaUWP2::UvMeasure> Position();
        Windows::UI::Xaml::UIElement Element();
        void Measure(Windows::Foundation::IReference<CelestiaUWP2::UvMeasure>);
        void Position(Windows::Foundation::IReference<CelestiaUWP2::UvMeasure>);
        void Element(Windows::UI::Xaml::UIElement const&);

        Windows::Foundation::IReference<CelestiaUWP2::UvMeasure> measure{ std::nullopt };
        Windows::Foundation::IReference<CelestiaUWP2::UvMeasure> position{ std::nullopt };
        Windows::UI::Xaml::UIElement element{ nullptr };
    private:
        int32_t index;
    };

    struct WrapLayoutState : WrapLayoutStateT<WrapLayoutState>
    {
        WrapLayoutState(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context);
        Windows::UI::Xaml::Controls::Orientation Orientation();
        Windows::Foundation::IReference<CelestiaUWP2::UvMeasure> Spacing();
        void Spacing(Windows::Foundation::IReference<CelestiaUWP2::UvMeasure>);
        double AvailableU();
        void AvailableU(double);

        CelestiaUWP2::WrapItem GetItemAt(int32_t index);
        void Clear();
        void RemoveFromIndex(int32_t index);
        void SetOrientation(Windows::UI::Xaml::Controls::Orientation orientation);
        void ClearPositions();
        double GetHeight();
        void RecycleElementAt(int32_t index);

        Windows::UI::Xaml::Controls::Orientation orientation{ Windows::UI::Xaml::Controls::Orientation::Horizontal };
        Windows::Foundation::IReference<CelestiaUWP2::UvMeasure> spacing{ std::nullopt };
        double availableU{ 0.0 };
    private:
        Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext context;
        std::vector<CelestiaUWP2::WrapItem> items;
    };

    struct WrapLayout : WrapLayoutT<WrapLayout>
    {
        WrapLayout();

        double HorizontalSpacing();
        void HorizontalSpacing(double);
        double VerticalSpacing();
        void VerticalSpacing(double);
        Windows::UI::Xaml::Controls::Orientation Orientation();
        void Orientation(Windows::UI::Xaml::Controls::Orientation);

        void InitializeForContextCore(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context);
        void UninitializeForContextCore(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context);
        void OnItemsChangedCore(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context, Windows::Foundation::IInspectable const& source, Windows::UI::Xaml::Interop::NotifyCollectionChangedEventArgs const& args);
        Windows::Foundation::Size MeasureOverride(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context, Windows::Foundation::Size availableSize);
        Windows::Foundation::Size ArrangeOverride(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context, Windows::Foundation::Size finalSize);

        static Windows::UI::Xaml::DependencyProperty HorizontalSpacingProperty() { return horizontalSpacingProperty; }
        static Windows::UI::Xaml::DependencyProperty VerticalSpacingProperty() { return verticalSpacingProperty; }
        static Windows::UI::Xaml::DependencyProperty OrientationProperty() { return orientationProperty; }

        static void LayoutPropertyChanged(Windows::UI::Xaml::DependencyObject const&, Windows::UI::Xaml::DependencyPropertyChangedEventArgs const&);

    private:
        static Windows::UI::Xaml::DependencyProperty horizontalSpacingProperty;
        static Windows::UI::Xaml::DependencyProperty verticalSpacingProperty;
        static Windows::UI::Xaml::DependencyProperty orientationProperty;
    };
}

namespace winrt::CelestiaUWP2::factory_implementation
{
    struct WrapItem : WrapItemT<WrapItem, implementation::WrapItem>
    {
    };

    struct WrapLayoutState : WrapLayoutStateT<WrapLayoutState, implementation::WrapLayoutState>
    {
    };

    struct WrapLayout : WrapLayoutT<WrapLayout, implementation::WrapLayout>
    {
    };
}