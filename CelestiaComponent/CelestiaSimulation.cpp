#include "pch.h"
#include "CelestiaSimulation.h"
#if __has_include("CelestiaSimulation.g.cpp")
#include "CelestiaSimulation.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaSimulation::CelestiaSimulation(Simulation* sim) : CelestiaSimulationT<CelestiaSimulation>(), sim(sim)
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
}
