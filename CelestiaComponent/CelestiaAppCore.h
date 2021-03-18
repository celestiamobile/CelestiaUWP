#pragma once

#include "CelestiaDestination.h"
#include "CelestiaSimulation.h"
#include "CelestiaScript.h"
#include "CelestiaAppCore.g.h"
#include <celestia/celestiacore.h>

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaAppCore : CelestiaAppCoreT<CelestiaAppCore>
    {
        CelestiaAppCore();

        bool StartSimulation(hstring const& configFileName, array_view<hstring const> extraDirectories, CelestiaComponent::CelestiaLoadCallback const &callback);
        bool StartRenderer();
        void Tick();
        void Draw();
        void Resize(int32_t width, int32_t height);
        void Start();
        void SetDPI(int32_t dpi);
        void MouseMove(float x, float y, int32_t button);
        void MouseButtonUp(float x, float y, int32_t button);
        void MouseButtonDown(float x, float y, int32_t button);
        void CharEnter(int16_t input);
        void SetContextMenuHandler(CelestiaComponent::CelestiaContextMenuCallback const& handler);
        void RunScript(hstring const& path);
        com_array<CelestiaComponent::CelestiaDestination> Destinations();
        CelestiaComponent::CelestiaSimulation Simulation();

        static void InitGL();
        static void SetLocaleDirectory(hstring const& localeDirectory);
        static hstring LocalizedString(hstring const& original);
        static com_array<CelestiaComponent::CelestiaScript> ReadScripts(hstring const& directory, bool deepScan);

    private:
        CelestiaCore* core;
        CelestiaComponent::CelestiaSimulation sim;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaAppCore : CelestiaAppCoreT<CelestiaAppCore, implementation::CelestiaAppCore>
    {
    };
}
