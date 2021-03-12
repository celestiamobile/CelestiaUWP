#pragma once

#include <celestia/celestiacore.h>

using namespace Platform;

namespace CelestiaComponent
{
    public delegate void CelestiaLoadCallback(Platform::String^ arg);
    public ref class CelestiaAppCore sealed
    {
    public:
        CelestiaAppCore();

        bool StartSimulation(String^ configFileName, const Platform::Array<String^>^ extraDirectories, CelestiaLoadCallback^ loadCallback);
        bool StartRenderer();
        void Tick();
        void Draw();
        void Resize(int width, int height);
        void Start();
        void SetDPI(int dpi);

        static void InitGL();
        static void SetLocaleDirectory(String^ localeDirectory);

    private:
        CelestiaCore* core;
    };
}
