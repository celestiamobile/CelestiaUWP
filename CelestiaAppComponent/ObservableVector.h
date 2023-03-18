#pragma once

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt::CelestiaAppComponent::implementation
{
    template <typename T>
    struct ObservableVectorIterator : implements<ObservableVectorIterator<T>,
        Windows::UI::Xaml::Interop::IBindableIterator>
    {
        ObservableVectorIterator(Windows::Foundation::Collections::IIterator<T> const& iterator) : iterator(iterator) {}

        bool HasCurrent()
        {
            return iterator.HasCurrent();
        }

        Windows::Foundation::IInspectable Current()
        {
            return iterator.Current();
        }

        bool MoveNext()
        {
            return iterator.MoveNext();
        }

    private:
        Windows::Foundation::Collections::IIterator<T> iterator;
    };

    template <typename T>
    struct VectorView : implements<VectorView<T>,
        Windows::UI::Xaml::Interop::IBindableVectorView,
        Windows::UI::Xaml::Interop::IBindableIterable>
    {
        VectorView(Windows::Foundation::Collections::IVectorView<T> const& store) : store(store) {}

        Windows::UI::Xaml::Interop::IBindableIterator First()
        {
            return make<ObservableVectorIterator<T>>(store.First());
        }

        Windows::Foundation::IInspectable GetAt(uint32_t const& index)
        {
            return store.GetAt(index);
        }

        uint32_t Size()
        {
            return store.Size();
        }

        bool IndexOf(Windows::Foundation::IInspectable const& value, uint32_t& index)
        {
            return store.IndexOf(value.as<T>(), index);
        }

    private:
        Windows::Foundation::Collections::IVectorView<T> store;
    };

    template <typename T>
    struct ObservableVector : implements<ObservableVector<T>,
        Windows::UI::Xaml::Interop::IBindableObservableVector,
        Windows::UI::Xaml::Interop::IBindableVector,
        Windows::UI::Xaml::Interop::IBindableIterable>
    {
        ObservableVector(Windows::Foundation::Collections::IObservableVector<T> const& store) : store(store)
        {
            store.VectorChanged([weak_this{ get_weak() }](Windows::Foundation::IInspectable const&, Windows::Foundation::Collections::IVectorChangedEventArgs const& args)
                {
                    auto strong_this{ weak_this.get() };
                    if (strong_this == nullptr) return;
                    strong_this->changed(*strong_this, args);
                });
        }

        event_token VectorChanged(Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler const& handler)
        {
            return changed.add(handler);
        }

        void VectorChanged(event_token const cookie)
        {
            changed.remove(cookie);
        }

        Windows::UI::Xaml::Interop::IBindableIterator First()
        {
            return make<ObservableVectorIterator<T>>(store.First());
        }

        Windows::Foundation::IInspectable GetAt(uint32_t const& index)
        {
            return store.GetAt(index);
        }

        void SetAt(uint32_t const index, Windows::Foundation::IInspectable const& value)
        {
            store.SetAt(index, value.as<T>());
        }

        void InsertAt(uint32_t const index, Windows::Foundation::IInspectable const& value)
        {
            store.InsertAt(index, value.as<T>());
        }

        void RemoveAt(uint32_t const index)
        {
            store.RemoveAt(index);
        }

        void Append(Windows::Foundation::IInspectable const& value)
        {
            store.Append(value.as<T>());
        }

        void RemoveAtEnd()
        {
            store.RemoveAtEnd();
        }

        void Clear()
        {
            store.Clear();
        }

        bool IndexOf(Windows::Foundation::IInspectable const& value, uint32_t& index)
        {
            return store.IndexOf(value.as<T>(), index);
        }

        Windows::UI::Xaml::Interop::IBindableVectorView GetView()
        {
            return make<VectorView<T>>(store.GetView());
        }

        uint32_t Size()
        {
            return store.Size();
        }

    private:
        Windows::Foundation::Collections::IObservableVector<T> store;
        event<Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler> changed;
    };
}