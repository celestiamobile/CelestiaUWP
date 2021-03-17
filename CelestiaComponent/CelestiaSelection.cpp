#include "pch.h"
#include "CelestiaSelection.h"
#if __has_include("CelestiaSelection.g.cpp")
#include "CelestiaSelection.g.cpp"
#endif

#include "CelestiaStar.h"
#include "CelestiaBody.h"
#include "CelestiaDSO.h"
#include "CelestiaLocation.h"

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaSelection::CelestiaSelection(CelestiaComponent::CelestiaAstroObject const& obj) : CelestiaSelectionT<CelestiaSelection>(), s(nullptr)
    {
        auto interfaces = get_interfaces(obj);
        auto object = get_self<CelestiaAstroObject>(obj)->obj;

        auto &bodyGuid = guid_of<CelestiaBody>();
        auto &dsoGuid = guid_of<CelestiaDSO>();
        auto &starGuid = guid_of<CelestiaStar>();
        auto &locationGuid = guid_of<CelestiaLocation>();

        for (auto &interf : interfaces)
        {
            if (interf == bodyGuid)
            {
                s = new Selection(reinterpret_cast<Body*>(object));
                return;
            }
            if (interf == starGuid)
            {
                s = new Selection(reinterpret_cast<Star*>(object));
                return;
            }
            if (interf == dsoGuid)
            {
                s = new Selection(reinterpret_cast<DeepSkyObject*>(object));
                return;
            }
            if (interf == locationGuid)
            {
                s = new Selection(reinterpret_cast<Location*>(object));
                return;
            }
        }
        s = nullptr;
    }

    CelestiaSelection::CelestiaSelection(Selection const& sel) : CelestiaSelectionT<CelestiaSelection>(), s(new Selection(sel))
    {
    }

    CelestiaComponent::CelestiaAstroObject CelestiaSelection::Object()
    {
        switch (s->getType())
        {
        case Selection::Type_Star:
            return make<CelestiaComponent::implementation::CelestiaStar>(s->star());
        case Selection::Type_DeepSky:
            return make<CelestiaComponent::implementation::CelestiaDSO>(s->deepsky());
        case Selection::Type_Body:
            return make<CelestiaComponent::implementation::CelestiaBody>(s->body());
        case Selection::Type_Location:
            return make<CelestiaComponent::implementation::CelestiaLocation>(s->location());
        default:
            return nullptr;
        }
    }

    CelestiaSelection::~CelestiaSelection()
    {
        delete s;
    }
}
