// BookmarkHelper.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "BookmarkHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct BookmarkHelper : BookmarkHelperT<BookmarkHelper>
    {
        static Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode>> ReadBookmarks();
        static Windows::Foundation::IAsyncAction WriteBookmarks(Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> const& bookmarks);
        static Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> CreateEmptyList();
        static Microsoft::UI::Xaml::Interop::IBindableObservableVector ConvertToBindable(Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> const& list);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct BookmarkHelper : BookmarkHelperT<BookmarkHelper, implementation::BookmarkHelper>
    {
    };
}
