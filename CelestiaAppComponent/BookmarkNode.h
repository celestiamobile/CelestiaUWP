#pragma once

#include "BookmarkNode.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct BookmarkNode : BookmarkNodeT<BookmarkNode>
    {
        BookmarkNode(Windows::Data::Json::JsonObject const& jsonObject) noexcept;
        BookmarkNode(bool isFolder, hstring const& name, hstring const& url, Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> children) noexcept;

        bool IsFolder();
        void IsFolder(bool);

        hstring Name();
        void Name(hstring const&);

        hstring URL();
        void URL(hstring const&);

        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> Children();
        void Children(Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> const&);
        Windows::UI::Xaml::Interop::IBindableObservableVector BindableChildren();

        bool HasErrors() noexcept;
        Windows::Data::Json::JsonObject JsonRepresentation() noexcept;

    private:
        bool isFolder;
        hstring name;
        hstring url;
        Windows::Foundation::Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> children;
        Windows::UI::Xaml::Interop::IBindableObservableVector bindableChildren;
        bool hasErrors;
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct BookmarkNode : BookmarkNodeT<BookmarkNode, implementation::BookmarkNode>
    {
    };
}
