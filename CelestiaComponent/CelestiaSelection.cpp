#include "pch.h"
#include "CelestiaSelection.h"
#if __has_include("CelestiaSelection.g.cpp")
#include "CelestiaSelection.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaSelection::CelestiaSelection(CelestiaComponent::CelestiaAstroObject const& obj) : CelestiaSelectionT<CelestiaSelection>()
    {
    }

    CelestiaSelection::CelestiaSelection(Selection const& sel) : CelestiaSelectionT<CelestiaSelection>(), s(new Selection(sel))
    {
    }

    CelestiaComponent::CelestiaBody CelestiaSelection::Body()
    {
        if (s->getType() != Selection::Type_Body)
            return nullptr;
        return make<CelestiaComponent::implementation::CelestiaBody>(s->body());
    }

    CelestiaSelection::~CelestiaSelection()
    {
        delete s;
    }
}
