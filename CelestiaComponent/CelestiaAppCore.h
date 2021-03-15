#pragma once

#include "CelestiaAppCore.g.h"
#include <celestia/celestiacore.h>

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaAppCore : CelestiaAppCoreT<CelestiaAppCore>
    {
        CelestiaAppCore();

        bool StartSimulation(hstring configFileName, array_view<hstring const> extraDirectories, CelestiaComponent::CelestiaLoadCallback const &callback);
        bool StartRenderer();
        void Tick();
        void Draw();
        void Resize(int32_t width, int32_t height);
        void Start();
        void SetDPI(int32_t dpi);

        static void InitGL();
        static void SetLocaleDirectory(hstring localeDirectory);

    private:
        CelestiaCore* core;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaAppCore : CelestiaAppCoreT<CelestiaAppCore, implementation::CelestiaAppCore>
    {
    };
}
