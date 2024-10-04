#include "pch.h"
#include "BookmarkNode.h"
#if __has_include("BookmarkNode.g.cpp")
#include "BookmarkNode.g.cpp"
#endif

#include "ObservableVector.h"

using namespace std;
using namespace winrt;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;

namespace winrt::CelestiaAppComponent::implementation
{
    BookmarkNode::BookmarkNode(JsonObject const& jsonObject) noexcept : isFolder(false), name(L""), url(L""), children(single_threaded_observable_vector<CelestiaAppComponent::BookmarkNode>())
    {
        bindableChildren = make<ObservableVector<CelestiaAppComponent::BookmarkNode>>(children);
        name = jsonObject.GetNamedString(L"Name", name);
        url = jsonObject.GetNamedString(L"URL", url);
        isFolder = jsonObject.GetNamedBoolean(L"IsFolder", isFolder);
        hasErrors = false;
        if (isFolder && jsonObject.HasKey(L"Children"))
        {
            try
            {
                auto childrenValue = jsonObject.GetNamedArray(L"Children");
                for (const auto& object : childrenValue)
                {
                    auto generatedObject = CelestiaAppComponent::BookmarkNode(object.GetObject());
                    if (generatedObject.HasErrors())
                    {
                        hasErrors = true;
                        break;
                    }
                    children.Append(generatedObject);
                }
            }
            catch (hresult_error const&)
            {
                hasErrors = true;
            }
        }
    }

    BookmarkNode::BookmarkNode(bool isFolder, hstring const& name, hstring const& url, Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> children) noexcept : isFolder(isFolder), name(name), url(url), children(children), hasErrors(false)
    {
        bindableChildren = make<ObservableVector<CelestiaAppComponent::BookmarkNode>>(children);
    }

    bool BookmarkNode::IsFolder()
    {
        return isFolder;
    }

    void BookmarkNode::IsFolder(bool value)
    {
        isFolder = value;
    }

    hstring BookmarkNode::Name()
    {
        return name;
    }

    void BookmarkNode::Name(hstring const& value)
    {
        name = value;
    }

    hstring BookmarkNode::URL()
    {
        return url;
    }

    void BookmarkNode::URL(hstring const& value)
    {
        url = value;
    }

    Windows::UI::Xaml::Interop::IBindableObservableVector BookmarkNode::BindableChildren()
    {
        return bindableChildren;
    }

    Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> BookmarkNode::Children()
    {
        return children;
    }

    void BookmarkNode::Children(Collections::IObservableVector<CelestiaAppComponent::BookmarkNode> const& value)
    {
        children = value;
    }

    bool BookmarkNode::HasErrors() noexcept
    {
        return hasErrors;
    }

    JsonObject BookmarkNode::JsonRepresentation() noexcept
    {
        JsonObject object;
        object.SetNamedValue(L"Name", JsonValue::CreateStringValue(name));
        object.SetNamedValue(L"URL", JsonValue::CreateStringValue(url));
        object.SetNamedValue(L"IsFolder", JsonValue::CreateBooleanValue(isFolder));
        if (children.Size() > 0)
        {
            JsonArray jsonArray;
            for (const auto& child : children)
            {
                jsonArray.Append(child.JsonRepresentation());
            }
            object.SetNamedValue(L"Children", jsonArray);
        }
        return object;
    }
}
