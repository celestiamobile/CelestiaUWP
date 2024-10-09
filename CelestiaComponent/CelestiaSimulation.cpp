//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include <celmath/geomutil.h>
#include "CelestiaHelper.h"
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
		sim->setSelection(get_self<CelestiaSelection>(selection)->AsSelection());
	}

    Windows::Foundation::DateTime CelestiaSimulation::Time()
    {
        return CelestiaHelper::DateTimeFromJulianDay(sim->getTime());
    }

    void CelestiaSimulation::Time(Windows::Foundation::DateTime const& time)
    {
        sim->setTime(CelestiaHelper::JulianDayFromDateTime(time));
    }

    double CelestiaSimulation::JulianDay()
    {
        return sim->getTime();
    }

    void CelestiaSimulation::JulianDay(double jd)
    {
        sim->setTime(jd);
    }

	CelestiaComponent::CelestiaUniverse CelestiaSimulation::Universe()
	{
		if (u == nullptr)
			u = make<CelestiaUniverse>(sim->getUniverse());
		return u;
	}

    CelestiaComponent::CelestiaObserver CelestiaSimulation::ActiveObserver()
    {
        return make<CelestiaObserver>(sim->getActiveObserver());
    }

    CelestiaComponent::CelestiaSelection CelestiaSimulation::Find(hstring const& name)
    {
        return make<CelestiaSelection>(sim->findObjectFromPath(to_string(name), true));
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
            case CelestiaComponent::CelestiaGotoLocationDistanceUnit::KM:
                distance = distanceValue;
            case CelestiaComponent::CelestiaGotoLocationDistanceUnit::AU:
                distance = celestia::astro::AUtoKilometers(distanceValue);
            case CelestiaComponent::CelestiaGotoLocationDistanceUnit::Radii:
                distance = radius * distanceValue;
            }
        }

        Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
        if (loc->longitude && loc->latitude)
        {
            sim->gotoSelectionLongLat(5, distance, loc->longitude.Value() * (float)M_PI / 180.0f, loc->latitude.Value() * (float)M_PI / 180.0f, up);
        }
        else
        {
            sim->gotoSelection(5, distance, up, ObserverFrame::ObserverLocal);
        }
    }

    void CelestiaSimulation::GoToEclipse(CelestiaComponent::CelestiaEclipse const& eclipse)
    {
        auto e = get_self<CelestiaEclipse>(eclipse);
        auto o = get_self<CelestiaBody>(e->Occulter());
        auto r = get_self<CelestiaBody>(e->Receiver());
        Star* star = reinterpret_cast<Body*>(r->obj)->getSystem()->getStar();
        if (!star)
            return;

        auto target = ::Selection(reinterpret_cast<Body*>(r->obj));
        auto ref = ::Selection(star);

        if (target.empty() || ref.empty())
            return;

        sim->setTime(e->startTime);
        sim->setFrame(ObserverFrame::PhaseLock, target, ref);
        sim->update(0);
        double distance = target.radius() * 4.0;
        sim->gotoLocation(UniversalCoord::Zero().offsetKm(Eigen::Vector3d::UnitX() * distance),
            celestia::math::YRotation(-0.5 * celestia::numbers::pi) * celestia::math::XRotation(-0.5 * celestia::numbers::pi),
            2.5);
    }

    void CelestiaSimulation::ReverseOrientation()
    {
        sim->reverseObserverOrientation();
    }

    com_array<CelestiaComponent::CelestiaCompletion> CelestiaSimulation::GetCompletion(hstring const& name)
    {
        std::vector<CelestiaComponent::CelestiaCompletion> vec;
        std::vector<celestia::engine::Completion> results;
        sim->getObjectCompletion(results, to_string(name), true);
        for (const auto& result : results)
            vec.push_back(make<CelestiaCompletion>(result));
        return com_array<CelestiaComponent::CelestiaCompletion>(vec);
    }
}
