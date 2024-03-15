//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "DataHelper.h"
#if __has_include("DataHelper.g.cpp")
#include "DataHelper.g.cpp"
#endif

#include "ObservableVector.h"

using namespace std;

namespace winrt::CelestiaAppComponent::implementation
{
    Windows::UI::Xaml::Interop::IBindableObservableVector DataHelper::ConvertToBindableDestinations(Windows::Foundation::Collections::IObservableVector<CelestiaComponent::CelestiaDestination> const& list)
    {
        return make<ObservableVector<CelestiaComponent::CelestiaDestination>>(list);
    }
}
