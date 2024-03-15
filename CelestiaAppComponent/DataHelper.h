//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "DataHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct DataHelper : DataHelperT<DataHelper>
    {
        static Windows::UI::Xaml::Interop::IBindableObservableVector ConvertToBindableDestinations(Windows::Foundation::Collections::IObservableVector<CelestiaComponent::CelestiaDestination> const& list);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct DataHelper : DataHelperT<DataHelper, implementation::DataHelper>
    {
    };
}
