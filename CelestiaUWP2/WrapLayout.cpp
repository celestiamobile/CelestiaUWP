//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"

#include "WrapLayout.h"
#if __has_include("WrapItem.g.cpp")
#include "WrapItem.g.cpp"
#endif
#if __has_include("WrapLayoutState.g.cpp")
#include "WrapLayoutState.g.cpp"
#endif
#if __has_include("WrapLayout.g.cpp")
#include "WrapLayout.g.cpp"
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

#ifndef NOMINMAX
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif
#endif

namespace winrt::CelestiaUWP2::implementation
{
    CelestiaUWP2::UvBounds CreateUvBounds(Controls::Orientation orientation, Rect const& rect)
    {
        CelestiaUWP2::UvBounds bounds;
        if (orientation == Controls::Orientation::Horizontal)
        {
            bounds.UMin = static_cast<double>(rect.X);
            bounds.UMax = static_cast<double>(rect.X + rect.Width);
            bounds.VMin = static_cast<double>(rect.Y);
            bounds.VMax = static_cast<double>(rect.Y + rect.Height);
        }
        else
        {
            bounds.UMin = static_cast<double>(rect.Y);
            bounds.UMax = static_cast<double>(rect.Y + rect.Height);
            bounds.VMin = static_cast<double>(rect.X);
            bounds.VMax = static_cast<double>(rect.X + rect.Width);
        }
        return bounds;
    }

    CelestiaUWP2::UvMeasure CreateUvMeasure(Controls::Orientation orientation, Size const& size)
    {
        CelestiaUWP2::UvMeasure measure;
        if (orientation == Controls::Orientation::Horizontal)
        {
            measure.U = static_cast<double>(size.Width);
            measure.V = static_cast<double>(size.Height);
        }
        else
        {
            measure.U = static_cast<double>(size.Height);
            measure.V = static_cast<double>(size.Width);
        }
        return measure;
    }

    Point UvMeasureGetPoint(CelestiaUWP2::UvMeasure const& measure, Controls::Orientation orientation)
    {
        return orientation == Controls::Orientation::Horizontal ? Point(static_cast<float>(measure.U), static_cast<float>(measure.V)) : Point(static_cast<float>(measure.V), static_cast<float>(measure.U));
    }

    Size UvMeasureGetSize(CelestiaUWP2::UvMeasure const& measure, Controls::Orientation orientation)
    {
        return orientation == Controls::Orientation::Horizontal ? Size(static_cast<float>(measure.U), static_cast<float>(measure.V)) : Size(static_cast<float>(measure.V), static_cast<float>(measure.U));
    }

    bool UvMeasureEquals(CelestiaUWP2::UvMeasure const& lhs, IReference<CelestiaUWP2::UvMeasure> const& rhs)
    {
        if (!rhs) return false;
        return lhs.U == rhs.Value().U && lhs.V == rhs.Value().V;
    }

    WrapItem::WrapItem(int32_t index) : index(index) {}
    int32_t WrapItem::Index() { return index; }
    IReference<CelestiaUWP2::UvMeasure> WrapItem::Measure() { return measure; }
    IReference<CelestiaUWP2::UvMeasure> WrapItem::Position() { return position; }
    void WrapItem::Measure(IReference<CelestiaUWP2::UvMeasure> value) { measure = value; }
    void WrapItem::Position(IReference<CelestiaUWP2::UvMeasure> value) { position = value; }
    void WrapItem::Element(Windows::UI::Xaml::UIElement const& value) { element = value; }
    UIElement WrapItem::Element() { return element; }

    WrapLayoutState::WrapLayoutState(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context) : context(context) {}
    Controls::Orientation WrapLayoutState::Orientation() { return orientation; }
    IReference<CelestiaUWP2::UvMeasure> WrapLayoutState::Spacing() { return spacing; }
    double WrapLayoutState::AvailableU() { return availableU; }
    void WrapLayoutState::Spacing(IReference<CelestiaUWP2::UvMeasure> value) { spacing = value; }
    void WrapLayoutState::AvailableU(double value) { availableU = value; };

    CelestiaUWP2::WrapItem WrapLayoutState::GetItemAt(int32_t index)
    {
        if (index < items.size())
        {
            return items[index];
        }
        else
        {
            CelestiaUWP2::WrapItem item{ index };
            items.push_back(item);
            return item;
        }
    }

    void WrapLayoutState::Clear()
    {
        items.clear();
    }

    void WrapLayoutState::RemoveFromIndex(int32_t index)
    {
        if (index >= items.size())
        {
            // Item was added/removed but we haven't realized that far yet
            return;
        }

        items.erase(items.begin() + index, items.end());
    }

    void WrapLayoutState::SetOrientation(Controls::Orientation newOrientation)
    {
        for(auto const& item : items)
        {
            if (!item.Measure())
                continue;
            auto measure = item.Measure().Value();
            std::swap(measure.U, measure.V);
            item.Position(measure);
        }

        this->orientation = newOrientation;
        this->availableU = 0.0;
    }

    void WrapLayoutState::ClearPositions()
    {
        for (auto const& item : items)
        {
            item.Position(IReference<CelestiaUWP2::UvMeasure>(std::nullopt));
        }
    }

    double WrapLayoutState::GetHeight()
    {
        if (items.empty())
            return 0;

        IReference<CelestiaUWP2::UvMeasure> lastPosition{ std::nullopt };
        double maxV = 0;

        for (auto i = static_cast<int32_t>(items.size()) - 1; i >= 0; --i)
        {
            auto item = items[i];
            if (!item.Position() || !item.Measure())
                continue;

            if (lastPosition && lastPosition.Value().V > item.Position().Value().V)
            {
                // This is a row above the last item.
                break;
            }

            lastPosition = item.Position();
            maxV = max(maxV, item.Measure().Value().V);
        }

        return lastPosition ? lastPosition.Value().V + maxV : maxV;
    }

    void WrapLayoutState::RecycleElementAt(int32_t index)
    {
        auto element = context.GetOrCreateElementAt(index);
        context.RecycleElement(element);
    }

    double WrapLayout::HorizontalSpacing()
    {
        return winrt::unbox_value<double>(GetValue(horizontalSpacingProperty));
    }

    void WrapLayout::HorizontalSpacing(double value)
    {
        SetValue(horizontalSpacingProperty, winrt::box_value(value));
    }

    double WrapLayout::VerticalSpacing()
    {
        return winrt::unbox_value<double>(GetValue(verticalSpacingProperty));
    }

    void WrapLayout::VerticalSpacing(double value)
    {
        SetValue(verticalSpacingProperty, winrt::box_value(value));
    }

    Controls::Orientation WrapLayout::Orientation()
    {
        return winrt::unbox_value<Controls::Orientation>(GetValue(orientationProperty));
    }

    WrapLayout::WrapLayout() {}

    void WrapLayout::Orientation(Controls::Orientation value)
    {
        SetValue(orientationProperty, winrt::box_value(value));
    }

    void WrapLayout::LayoutPropertyChanged(DependencyObject const& d, DependencyPropertyChangedEventArgs const&)
    {
        auto wp = d.try_as<WrapLayout>();
        if (wp != nullptr)
        {
            wp->InvalidateMeasure();
            wp->InvalidateArrange();
        }
    }

    void WrapLayout::InitializeForContextCore(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context)
    {
        context.LayoutState(CelestiaUWP2::WrapLayoutState(context));
        base_type::InitializeForContextCore(context);
    }

    void WrapLayout::UninitializeForContextCore(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context)
    {
        context.LayoutState(nullptr);
        base_type::UninitializeForContextCore(context);
    }

    void WrapLayout::OnItemsChangedCore(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context, IInspectable const& source, Interop::NotifyCollectionChangedEventArgs const& args)
    {
        auto state = context.LayoutState().as<CelestiaUWP2::WrapLayoutState>();
        
        switch (args.Action())
        {
        case Interop::NotifyCollectionChangedAction::Add:
            state.RemoveFromIndex(args.NewStartingIndex());
            break;
        case Interop::NotifyCollectionChangedAction::Move:
            {
                auto minIndex = min(args.OldStartingIndex(), args.NewStartingIndex());
                state.RemoveFromIndex(minIndex);

                state.RecycleElementAt(args.OldStartingIndex());
                state.RecycleElementAt(args.NewStartingIndex());
            }
            break;
        case Interop::NotifyCollectionChangedAction::Remove:
            state.RemoveFromIndex(args.OldStartingIndex());
            break;
        case Interop::NotifyCollectionChangedAction::Replace:
            state.RemoveFromIndex(args.NewStartingIndex());
            state.RecycleElementAt(args.NewStartingIndex());
            break;
        case Interop::NotifyCollectionChangedAction::Reset:
            state.Clear();
            break;
        }

        base_type::OnItemsChangedCore(context, source, args);
    }

    Size WrapLayout::MeasureOverride(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context, Size availableSize)
    {
        auto parentMeasure = CreateUvMeasure(Orientation(), availableSize);
        auto spacingMeasure = CreateUvMeasure(Orientation(), Size(static_cast<float>(HorizontalSpacing()), static_cast<float>(VerticalSpacing())));

        auto state = context.LayoutState().as<CelestiaUWP2::WrapLayoutState>();
        if (state.Orientation() != Orientation())
        {
            state.SetOrientation(Orientation());
        }

        if (!UvMeasureEquals(spacingMeasure, state.Spacing()) || state.AvailableU() != parentMeasure.U)
        {
            state.ClearPositions();
            state.Spacing(spacingMeasure);
            state.AvailableU(parentMeasure.U);
        }

        double currentV = 0;
        auto realizationBounds = CreateUvBounds(Orientation(), context.RealizationRect());
        CelestiaUWP2::UvMeasure position;
        position.U = 0.0;
        position.V = 0.0;
        for (int32_t i = 0; i < context.ItemCount(); ++i)
        {
            bool measured = false;
            auto item = state.GetItemAt(i);
            if (!item.Measure())
            {
                item.Element(context.GetOrCreateElementAt(i));
                item.Element().Measure(availableSize);
                item.Measure(CreateUvMeasure(Orientation(), item.Element().DesiredSize()));
                measured = true;
            }

            auto currentMeasure = item.Measure().Value();

            if (!item.Position())
            {
                if (parentMeasure.U < position.U + currentMeasure.U)
                {
                    // New Row
                    position.U = 0;
                    position.V += currentV + spacingMeasure.V;
                    currentV = 0;
                }

                item.Position(position);
            }

            position = item.Position().Value();

            auto vEnd = position.V + currentMeasure.V;
            if (vEnd < realizationBounds.VMin)
            {
                // Item is "above" the bounds
                if (item.Element() != nullptr)
                {
                    context.RecycleElement(item.Element());
                    item.Element(nullptr);
                }

                continue;
            }
            else if (position.V > realizationBounds.VMax)
            {
                // Item is "below" the bounds.
                if (item.Element() != nullptr)
                {
                    context.RecycleElement(item.Element());
                    item.Element(nullptr);
                }

                // We don't need to measure anything below the bounds
                break;
            }
            else if (!measured)
            {
                // Always measure elements that are within the bounds
                item.Element(context.GetOrCreateElementAt(i));
                item.Element().Measure(availableSize);

                currentMeasure = CreateUvMeasure(Orientation(), item.Element().DesiredSize());
                if (!UvMeasureEquals(currentMeasure, item.Measure()))
                {
                    // this item changed size; we need to recalculate layout for everything after this
                    state.RemoveFromIndex(i + 1);
                    item.Measure(currentMeasure);

                    // did the change make it go into the new row?
                    if (parentMeasure.U < position.U + currentMeasure.U)
                    {
                        // New Row
                        position.U = 0;
                        position.V += currentV + spacingMeasure.V;
                        currentV = 0;
                    }

                    item.Position(position);
                }
            }

            position.U += currentMeasure.U + spacingMeasure.U;
            currentV = max(currentMeasure.V, currentV);
        }

        // update value with the last line
       // if the the last loop is(parentMeasure.U > currentMeasure.U + lineMeasure.U) the total isn't calculated then calculate it
       // if the last loop is (parentMeasure.U > currentMeasure.U) the currentMeasure isn't added to the total so add it here
       // for the last condition it is zeros so adding it will make no difference
       // this way is faster than an if condition in every loop for checking the last item
       // Propagating an infinite size causes a crash. This can happen if the parent is scrollable and infinite in the opposite
       // axis to the panel. Clearing to zero prevents the crash.
       // This is likely an incorrect use of the control by the developer, however we need stability here so setting a default that won't crash.
        CelestiaUWP2::UvMeasure totalMeasure;
        totalMeasure.U = std::isinf(parentMeasure.U) ? 0 : std::ceil(parentMeasure.U);
        totalMeasure.V = state.GetHeight();

        return UvMeasureGetSize(totalMeasure, Orientation());
    }

    Size WrapLayout::ArrangeOverride(Microsoft::UI::Xaml::Controls::VirtualizingLayoutContext const& context, Size finalSize)
    {
        if (context.ItemCount() > 0)
        {
            auto realizationBounds = CreateUvBounds(Orientation(), context.RealizationRect());

            auto state = context.LayoutState().as<CelestiaUWP2::WrapLayoutState>();

            for (int32_t i = 0; i < context.ItemCount(); ++i)
            {
                auto item = state.GetItemAt(i);
                if (!item.Position() || !item.Measure())
                    break;

                auto desiredMeasure = item.Measure().Value();
                auto position = item.Position().Value();

                if (realizationBounds.VMin <= position.V + desiredMeasure.V && position.V <= realizationBounds.VMax)
                {
                    // place the item
                    auto child = context.GetOrCreateElementAt(item.Index());
                    child.Arrange(Rect(UvMeasureGetPoint(position, Orientation()), UvMeasureGetSize(desiredMeasure, Orientation())));
                }
                else if (position.V > realizationBounds.VMax)
                {
                    break;
                }
            }
        }

        return finalSize;
    }

    DependencyProperty WrapLayout::horizontalSpacingProperty =
        Windows::UI::Xaml::DependencyProperty::Register(
            L"HorizontalSpacing",
            winrt::xaml_typename<double>(),
            winrt::xaml_typename<CelestiaUWP2::WrapLayout>(),
            Windows::UI::Xaml::PropertyMetadata{ winrt::box_value(0.0), PropertyChangedCallback{ &WrapLayout::LayoutPropertyChanged } }
    );

    DependencyProperty WrapLayout::verticalSpacingProperty =
        Windows::UI::Xaml::DependencyProperty::Register(
            L"VerticalSpacing",
            winrt::xaml_typename<double>(),
            winrt::xaml_typename<CelestiaUWP2::WrapLayout>(),
            Windows::UI::Xaml::PropertyMetadata{ winrt::box_value(0.0), PropertyChangedCallback{ &WrapLayout::LayoutPropertyChanged } }
    );

    DependencyProperty WrapLayout::orientationProperty =
        Windows::UI::Xaml::DependencyProperty::Register(
            L"Orientation",
            winrt::xaml_typename<Controls::Orientation>(),
            winrt::xaml_typename<CelestiaUWP2::WrapLayout>(),
            Windows::UI::Xaml::PropertyMetadata{ winrt::box_value(Controls::Orientation::Horizontal), PropertyChangedCallback{ &WrapLayout::LayoutPropertyChanged } }
    );
}