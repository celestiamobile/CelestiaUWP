#pragma once

#include <celestia/celestiacore.h>

namespace CelestiaComponent
{
    public ref class CelestiaAppCore sealed
    {
    public:
        CelestiaAppCore();
    private:
        CelestiaCore* core;
    };
}
