#pragma once

#include "BookmarkHelper.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct BookmarkHelper : BookmarkHelperT<BookmarkHelper>
    {
        static Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode>> ReadBookmarks();
        static Windows::Foundation::IAsyncAction WriteBookmarks(Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> const& bookmarks);
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct BookmarkHelper : BookmarkHelperT<BookmarkHelper, implementation::BookmarkHelper>
    {
    };
}
