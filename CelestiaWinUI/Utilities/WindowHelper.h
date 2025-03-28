#pragma once

#include "WindowHelper.g.h"

namespace winrt::CelestiaWinUI::implementation
{
    struct WindowHelper : WindowHelperT<WindowHelper>
    {
        static void ShowShareLink(Microsoft::UI::Xaml::Window const& window, hstring const& title, Windows::Foundation::Uri const& uri);
        static float GetWindowScaleFactor(Microsoft::UI::Xaml::Window const& window);
        static void TrackWindow(Microsoft::UI::Xaml::Window const& window, hstring const& key);
        static Microsoft::UI::Xaml::Window GetTrackedWindow(hstring const& key);
        static void SetWindowIcon(Microsoft::UI::Xaml::Window const& window);
        static void SetWindowTheme(Microsoft::UI::Xaml::Window const& window);
        static void SetWindowFlowDirection(Microsoft::UI::Xaml::Window const& window);
        static void ResizeWindow(Microsoft::UI::Xaml::Window const& window, int32_t width, int32_t height);

    private:
        static Windows::Foundation::Collections::IMap<hstring, Microsoft::UI::Xaml::Window> trackedWindows;
    };
}

namespace winrt::CelestiaWinUI::factory_implementation
{
    struct WindowHelper : WindowHelperT<WindowHelper, implementation::WindowHelper>
    {
    };
}
