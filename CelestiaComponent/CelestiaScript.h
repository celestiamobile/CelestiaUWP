#pragma once

#include <celestia/scriptmenu.h>
#include "CelestiaScript.g.h"

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaScript : CelestiaScriptT<CelestiaScript>
    {
        CelestiaScript(ScriptMenuItem const& script);

        hstring Filename();
        hstring Title();

        hstring filename;
        hstring title;
    };
}