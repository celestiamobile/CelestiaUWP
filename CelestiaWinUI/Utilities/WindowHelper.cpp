#include "pch.h"
#include "WindowHelper.h"
#if __has_include("WindowHelper.g.cpp")
#include "WindowHelper.g.cpp"
#endif

#include <windows.applicationmodel.datatransfer.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation;

namespace winrt::CelestiaWinUI::implementation
{
    void WindowHelper::ShowShareLink(Window const& window, hstring const& title, Uri const& uri)
    {
        using namespace Windows::ApplicationModel::DataTransfer;
        auto windowNative = window.try_as<::IWindowNative>();
        if (windowNative == nullptr) return;
        HWND hWnd{ 0 };
        windowNative->get_WindowHandle(&hWnd);
        auto transferManagerStatics = get_activation_factory<DataTransferManager, IDataTransferManagerStatics>();
        if (transferManagerStatics == nullptr) return;
        auto transferManagerInterop = transferManagerStatics.try_as<IDataTransferManagerInterop>();
        if (transferManagerInterop == nullptr) return;
        com_ptr<ABI::Windows::ApplicationModel::DataTransfer::IDataTransferManager> pDTM{ nullptr };
        transferManagerInterop->GetForWindow(hWnd, IID_PPV_ARGS(&pDTM));
        if (pDTM == nullptr) return;

        auto transferManager = pDTM.try_as<DataTransferManager>();
        if (transferManager == nullptr) return;

        transferManager.DataRequested([title, uri](Windows::Foundation::IInspectable const&, DataRequestedEventArgs const& args)
            {
                DataPackage dataPackage;
                dataPackage.Properties().Title(title);
                dataPackage.SetWebLink(uri);
                args.Request().Data(dataPackage);
            });
        transferManagerInterop->ShowShareUIForWindow(hWnd);
    }

    Microsoft::UI::Windowing::AppWindow WindowHelper::GetAppWindow(Window const& window)
    {
        auto windowNative{ window.try_as<::IWindowNative>() };
        if (windowNative)
        {
            HWND hWnd{ 0 };
            windowNative->get_WindowHandle(&hWnd);
            if (hWnd != 0)
            {
                auto windowId = Microsoft::UI::GetWindowIdFromWindow(hWnd);
                return Microsoft::UI::Windowing::AppWindow::GetFromWindowId(windowId);
            }
        }
        return nullptr;
    }

    float WindowHelper::GetWindowScaleFactor(Window const& window)
    {
        auto windowNative{ window.try_as<::IWindowNative>() };
        if (windowNative)
        {
            HWND hWnd{ 0 };
            windowNative->get_WindowHandle(&hWnd);
            if (hWnd != 0)
            {
                auto dpi = GetDpiForWindow(hWnd);;
                return dpi / 96.0f;
            }
        }
        return 1.0f;
    }

    void WindowHelper::TrackWindow(Window const& window, hstring const& key)
    {
        trackedWindows.Insert(key, window);
        window.Closed([key](IInspectable const&, WindowEventArgs const&)
            {
                trackedWindows.TryRemove(key);
            });
    }
   
    Window WindowHelper::GetTrackedWindow(hstring const& key)
    {
        return trackedWindows.TryLookup(key);
    }

    void WindowHelper::SetWindowIcon(Window const& window)
    {
        auto appWindow = GetAppWindow(window);
        if (appWindow)
        {
            appWindow.SetIcon(L"AppIcon.ico");
        }
    }

    void WindowHelper::SetWindowFlowDirection(Window const& window)
    {
        auto resourceLoader{ Windows::ApplicationModel::Resources::ResourceLoader::GetForViewIndependentUse() };
        auto flowDirection = resourceLoader.GetString(L"ApplicationFlowDirection");
        if (flowDirection != L"RightToLeft") return;

        auto windowNative{ window.try_as<IWindowNative>() };
        if (!windowNative) return;

        HWND hWnd{ 0 };
        windowNative->get_WindowHandle(&hWnd);
        auto extended_style = GetWindowLong(hWnd, GWL_EXSTYLE);
        SetWindowLong(hWnd, GWL_EXSTYLE, extended_style | WS_EX_LAYOUTRTL);

        auto content = window.Content().try_as<FrameworkElement>();
        if (content)
        {
            content.FlowDirection(FlowDirection::RightToLeft);
        }
    }

    void WindowHelper::ResizeWindow(Window const& window, int32_t width, int32_t height)
    {
        auto appWindow = GetAppWindow(window);
        if (appWindow)
        {
            auto scaleFactor = GetWindowScaleFactor(window);
            appWindow.Resize({ (int32_t)(width * scaleFactor), (int32_t)(height * scaleFactor) });
        }
    }

    Windows::Foundation::Collections::IMap<hstring, Microsoft::UI::Xaml::Window> WindowHelper::trackedWindows = single_threaded_observable_map<hstring, Window>();
}