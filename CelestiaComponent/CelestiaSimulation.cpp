#include "pch.h"
#include "CelestiaSimulation.h"
#if __has_include("CelestiaSimulation.g.cpp")
#include "CelestiaSimulation.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaSimulation::CelestiaSimulation(Simulation* sim) : CelestiaSimulationT<CelestiaSimulation>(), sim(sim), u(nullptr)
	{
	}

	CelestiaComponent::CelestiaSelection CelestiaSimulation::Selection()
	{
		return make<CelestiaSelection>(sim->getSelection());
	}

	void CelestiaSimulation::Selection(CelestiaComponent::CelestiaSelection const& selection)
	{
		sim->setSelection(*get_self<CelestiaSelection>(selection)->s);
	}

	CelestiaComponent::CelestiaUniverse CelestiaSimulation::Universe()
	{
		if (u == nullptr)
			u = make<CelestiaUniverse>(sim->getUniverse());
		return u;
	}

    CelestiaComponent::CelestiaSelection CelestiaSimulation::Find(hstring const& name)
    {
        return make<CelestiaSelection>(sim->findObject(to_string(name)));
    }

    void CelestiaSimulation::GoToDestination(CelestiaComponent::CelestiaDestination destination)
    {
        auto d = get_self<CelestiaDestination>(destination);
        auto sel = sim->findObjectFromPath(to_string(d->Target()));
        if (!sel.empty())
        {
            sim->follow();
            sim->setSelection(sel);
            if (d->Distance() <= 0)
            {
                // Use the default distance
                sim->gotoSelection(5.0,
                    Eigen::Vector3f::UnitY(),
                    ObserverFrame::ObserverLocal);
            }
            else
            {
                sim->gotoSelection(5.0,
                    d->Distance(),
                    Eigen::Vector3f::UnitY(),
                    ObserverFrame::ObserverLocal);
            }
        }
    }
}
