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
}
