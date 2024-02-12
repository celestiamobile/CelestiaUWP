#pragma once

#include "ResourceManager.g.h"
#include "ResourceManagerDownloadProgressArgs.g.h"
#include "ResourceManagerDownloadSuccessArgs.g.h"
#include "ResourceManagerDownloadFailureArgs.g.h"

namespace winrt::CelestiaAppComponent::implementation
{
    struct ResourceManagerDownloadProgressArgs : ResourceManagerDownloadProgressArgsT<ResourceManagerDownloadProgressArgs>
    {
        ResourceManagerDownloadProgressArgs(CelestiaAppComponent::ResourceItem const& item, double progress) : item(item), progress(progress) {};
        CelestiaAppComponent::ResourceItem Item() { return item; };
        double Progress() { return progress; };
    private:
        CelestiaAppComponent::ResourceItem item;
        double progress;
    };

    struct ResourceManagerDownloadSuccessArgs : ResourceManagerDownloadSuccessArgsT<ResourceManagerDownloadSuccessArgs>
    {
        ResourceManagerDownloadSuccessArgs(CelestiaAppComponent::ResourceItem const& item) : item(item) {};
        CelestiaAppComponent::ResourceItem Item() { return item; };
    private:
        CelestiaAppComponent::ResourceItem item;
    };

    struct ResourceManagerDownloadFailureArgs : ResourceManagerDownloadFailureArgsT<ResourceManagerDownloadFailureArgs>
    {
        ResourceManagerDownloadFailureArgs(CelestiaAppComponent::ResourceItem const& item, CelestiaAppComponent::ResourceErrorType errorType, hstring const& contextPath = L"") : item(item), errorType(errorType), contextPath(contextPath) {};
        CelestiaAppComponent::ResourceItem Item() { return item; };
        CelestiaAppComponent::ResourceErrorType ErrorType() { return errorType; };
        hstring ContextPath() { return contextPath; };
    private:
        CelestiaAppComponent::ResourceItem item;
        CelestiaAppComponent::ResourceErrorType errorType;
        hstring contextPath;
    };

    struct ResourceManager : ResourceManagerT<ResourceManager>
    {
        ResourceManager(Windows::Storage::StorageFolder const& addonFolder, Windows::Storage::StorageFolder const& scriptFolder);

        hstring ItemPath(CelestiaAppComponent::ResourceItem const& item);
        hstring ScriptPath(CelestiaAppComponent::ResourceItem const& item);
        CelestiaAppComponent::ResourceItemState StateForItem(CelestiaAppComponent::ResourceItem const& item);
        void Download(CelestiaAppComponent::ResourceItem const& item);
        Windows::Foundation::IAsyncAction DownloadAsync(CelestiaAppComponent::ResourceItem const item);
        void Cancel(CelestiaAppComponent::ResourceItem const& item);
        Windows::Foundation::IAsyncAction Uninstall(CelestiaAppComponent::ResourceItem const item);

        Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVector<CelestiaAppComponent::ResourceItem>> InstalledItems();

        event_token DownloadProgressUpdate(Windows::Foundation::EventHandler<CelestiaAppComponent::ResourceManagerDownloadProgressArgs> const&);
        void DownloadProgressUpdate(event_token const&);
        event_token DownloadSuccess(Windows::Foundation::EventHandler<CelestiaAppComponent::ResourceManagerDownloadSuccessArgs> const&);
        void DownloadSuccess(event_token const&);
        event_token DownloadFailure(Windows::Foundation::EventHandler<CelestiaAppComponent::ResourceManagerDownloadFailureArgs> const&);
        void DownloadFailure(event_token const&);

    private:
        Windows::Storage::StorageFolder addonFolder;
        Windows::Storage::StorageFolder scriptFolder;
        std::unordered_map<hstring, Windows::Foundation::IAsyncAction> tasks;

        event<Windows::Foundation::EventHandler<CelestiaAppComponent::ResourceManagerDownloadProgressArgs>> downloadProgressUpdateEvent;
        event<Windows::Foundation::EventHandler<CelestiaAppComponent::ResourceManagerDownloadSuccessArgs>> downloadSuccessEvent;
        event<Windows::Foundation::EventHandler<CelestiaAppComponent::ResourceManagerDownloadFailureArgs>> downloadFailureEvent;
    };
}

namespace winrt::CelestiaAppComponent::factory_implementation
{
    struct ResourceManager : ResourceManagerT<ResourceManager, implementation::ResourceManager>
    {
    };
}
