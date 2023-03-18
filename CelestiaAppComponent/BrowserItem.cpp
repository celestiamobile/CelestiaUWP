#include "pch.h"
#include "BrowserItem.h"
#if __has_include("BrowserItem.g.cpp")
#include "BrowserItem.g.cpp"
#endif
#if __has_include("BrowserItemTab.g.cpp")
#include "BrowserItemTab.g.cpp"
#endif

#include "ObservableVector.h"

using namespace std;
using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::CelestiaAppComponent::implementation
{
    BrowserItem::BrowserItem(CelestiaComponent::CelestiaBrowserItem const& item) : item(item)
    {
        children = single_threaded_observable_vector<CelestiaAppComponent::BrowserItem>();
    }

    CelestiaComponent::CelestiaBrowserItem BrowserItem::Item() { return item; }

    bool BrowserItem::IsExpanded()
    {
        return expanded;
    }

    void BrowserItem::IsExpanded(bool value)
    {
        expanded = value;
    }

    hstring BrowserItem::ItemName()
    {
        return item.Name();
    }

    Collections::IObservableVector<CelestiaAppComponent::BrowserItem> BrowserItem::Children()
    {
        if (!areChildrenLoaded)
        {
            for (const auto& child : item.Children())
            {
                children.Append(CelestiaAppComponent::BrowserItem(child));
            }
            areChildrenLoaded = true;
        }
        return children;
    }

    Windows::UI::Xaml::Interop::IBindableObservableVector BrowserItem::BindableChildren()
    {
        if (!areBindableChildrenLoaded)
        {
            bindableChildren = ConvertToBindable(Children());
            areBindableChildrenLoaded = true;
        }
        return bindableChildren;
    }

    Windows::UI::Xaml::Interop::IBindableObservableVector BrowserItem::ConvertToBindable(Collections::IObservableVector<CelestiaAppComponent::BrowserItem> const& list)
    {
        return make<ObservableVector<CelestiaAppComponent::BrowserItem>>(list);
    }

    BrowserItemTab::BrowserItemTab(Collections::IVector<CelestiaAppComponent::BrowserItem> const& children, hstring const& tabName) : tabName(tabName)
    {
        this->children = single_threaded_observable_vector<CelestiaAppComponent::BrowserItem>({ children.begin(), children.end() });
    }

    hstring BrowserItemTab::TabName()
    {
        return tabName;
    }

    Collections::IObservableVector<CelestiaAppComponent::BrowserItem> BrowserItemTab::Children()
    {
        return children;
    }
}
