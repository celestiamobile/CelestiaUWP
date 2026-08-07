// CelestiaSimulation.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include <celcompat/numbers.h>
#include <celmath/geomutil.h>
#include <celmath/intersect.h>
#include <celmath/sphere.h>
#include "CelestiaHelper.h"
#include "CelestiaSimulation.h"
#if __has_include("CelestiaSimulation.g.cpp")
#include "CelestiaSimulation.g.cpp"
#endif

using namespace std;

namespace
{

Eigen::Vector3d findMaxEclipsePoint(const Eigen::Vector3d& toOcculter,
                                    const Eigen::Vector3d& toReceiver,
                                    double receiverRadius)
{
    double distance = 0.0;
    if (celestia::math::testIntersection(
            Eigen::ParametrizedLine<double, 3>(Eigen::Vector3d::Zero(), toOcculter),
            celestia::math::Sphered(toReceiver, receiverRadius),
            distance))
    {
        return toOcculter * distance - toReceiver;
    }

    const double t = toReceiver.dot(toOcculter) / toOcculter.squaredNorm();
    Eigen::Vector3d point = t * toOcculter - toReceiver;
    return point * (receiverRadius / point.norm());
}

}

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
                    ObserverFrame::CoordinateSystem::ObserverLocal);
            }
            else
            {
                sim->gotoSelection(5.0,
                    d->Distance(),
                    Eigen::Vector3f::UnitY(),
                    ObserverFrame::CoordinateSystem::ObserverLocal);
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
        double distance = radius * 5.0;

        if (loc->distance && loc->unit)
        {
            double distanceValue = loc->distance.Value();
            switch (loc->unit.Value())
            {
            case CelestiaComponent::CelestiaGotoLocationDistanceUnit::KM:
                distance = distanceValue;
                break;
            case CelestiaComponent::CelestiaGotoLocationDistanceUnit::AU:
                distance = celestia::astro::AUtoKilometers(distanceValue);
                break;
            case CelestiaComponent::CelestiaGotoLocationDistanceUnit::Radii:
            default:
                distance = radius * distanceValue;
                break;
            }
        }

        Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
        if (loc->longitude && loc->latitude)
        {
            sim->gotoSelectionLongLat(5.0, distance, loc->longitude.Value() * static_cast<float>(celestia::numbers::pi) / 180.0f, loc->latitude.Value() * static_cast<float>(celestia::numbers::pi) / 180.0f, up);
        }
        else
        {
            sim->gotoSelection(5, distance, up, ObserverFrame::CoordinateSystem::ObserverLocal);
        }
    }

    void CelestiaSimulation::PerformEclipseAction(CelestiaComponent::CelestiaEclipse const& eclipse, CelestiaComponent::CelestiaEclipseAction action)
    {
        auto e = get_self<CelestiaEclipse>(eclipse);
        auto o = get_self<CelestiaBody>(e->Occulter());
        auto r = get_self<CelestiaBody>(e->Receiver());
        auto occulter = reinterpret_cast<Body*>(o->obj);
        auto receiver = reinterpret_cast<Body*>(r->obj);
        Star* sun = receiver->getSystem()->getStar();
        if (!sun)
            return;

        const double midEclipseTime = (e->startTime + e->endTime) / 2.0;
        if (action == CelestiaComponent::CelestiaEclipseAction::SetTime)
        {
            sim->setTime(midEclipseTime);
            return;
        }

        double now = sim->getTime();
        if (now < e->startTime || now > e->endTime)
            sim->setTime(midEclipseTime);
        now = sim->getTime();

        const Eigen::Vector3d toOcculter = occulter->getPosition(now).offsetFromKm(sun->getPosition(now));
        const Eigen::Vector3d toReceiver = receiver->getPosition(now).offsetFromKm(sun->getPosition(now));
        const Eigen::Vector3d receiverUp = receiver->getEclipticToBodyFixed(now).conjugate() * Eigen::Vector3d::UnitY();

        Body* frameBody = receiver;
        Eigen::Vector3d position;
        Eigen::Quaterniond orientation;

        switch (action)
        {
        case CelestiaComponent::CelestiaEclipseAction::NearEclipsedBody:
        {
            const Eigen::Vector3d eclipsePoint = findMaxEclipsePoint(toOcculter, toReceiver, receiver->getRadius());
            position = eclipsePoint * 4.0;
            orientation = celestia::math::LookAt<double>(position, eclipsePoint, receiverUp);
            break;
        }
        case CelestiaComponent::CelestiaEclipseAction::FromEclipsedBodySurface:
        {
            const Eigen::Vector3d eclipsePoint = findMaxEclipsePoint(toOcculter, toReceiver, receiver->getRadius());
            position = eclipsePoint * 1.0001;
            orientation = celestia::math::LookAt<double>(eclipsePoint, -toReceiver, eclipsePoint.normalized());
            break;
        }
        case CelestiaComponent::CelestiaEclipseAction::FromOcculterSurface:
        case CelestiaComponent::CelestiaEclipseAction::BehindOcculter:
            frameBody = occulter;
            position = toOcculter.normalized() * occulter->getRadius()
                * (action == CelestiaComponent::CelestiaEclipseAction::FromOcculterSurface ? 1.0001 : 20.0);
            orientation = celestia::math::LookAt<double>(position, toReceiver, receiverUp);
            break;
        case CelestiaComponent::CelestiaEclipseAction::SetTime:
            return;
        }

        sim->setFrame(ObserverFrame::CoordinateSystem::Ecliptical, frameBody);
        sim->gotoLocation(UniversalCoord::Zero().offsetKm(position), orientation, 5.0);
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
