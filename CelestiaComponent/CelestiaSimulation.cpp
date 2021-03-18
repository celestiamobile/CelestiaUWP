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

    void CelestiaSimulation::GoToDestination(CelestiaComponent::CelestiaDestination const& destination)
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

    void CelestiaSimulation::GoToLocation(CelestiaComponent::CelestiaGotoLocation const& location)
    {
        CelestiaGotoLocation* loc = get_self<CelestiaGotoLocation>(location);
        CelestiaSelection* sel = get_self<CelestiaSelection>(loc->selection);
        Selection(loc->selection);
        sim->geosynchronousFollow();
        double radius = sel->Radius();
        double distance = radius * 5;

        if (loc->distance && loc->unit)
        {
            double distanceValue = loc->distance.Value();
            switch (loc->unit.Value())
            {
            case CelestiaComponent::CelestiaGotoLocationDistanceUnit::km:
                distance = distanceValue;
            case CelestiaComponent::CelestiaGotoLocationDistanceUnit::au:
                distance = astro::AUtoKilometers(distanceValue);
            case CelestiaComponent::CelestiaGotoLocationDistanceUnit::radii:
                distance = radius * distanceValue;
            }
        }

        Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
        if (loc->longitude && loc->latitude)
        {
            sim->gotoSelectionLongLat(5, distance, (float)loc->longitude.Value() * M_PI / 180.0, (float)loc->latitude.Value() * M_PI / 180.0, up);
        }
        else
        {
            sim->gotoSelection(5, distance, up, ObserverFrame::ObserverLocal);
        }
    }
}
