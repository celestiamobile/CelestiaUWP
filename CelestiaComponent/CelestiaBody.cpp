//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaBody.h"
#include "CelestiaHelper.h"
#include "CelestiaPlanetarySystem.h"
#if __has_include("CelestiaBody.g.cpp")
#include "CelestiaBody.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaBody::CelestiaBody(Body* body) : CelestiaBodyT<CelestiaBody, CelestiaAstroObject>(body)
	{
	}

	hstring CelestiaBody::Name()
	{
		return to_hstring(static_cast<Body*>(obj)->getName(true));
	}

    CelestiaComponent::CelestiaBodyType CelestiaBody::Type()
    {
        return static_cast<CelestiaComponent::CelestiaBodyType>(static_cast<Body*>(obj)->getClassification());
    }

	com_array<hstring> CelestiaBody::AlternateSurfaceNames()
	{
		auto altSurfaces = GetBodyFeaturesManager()->getAlternateSurfaceNames(static_cast<Body*>(obj));
		std::vector<hstring> results;
		if (altSurfaces.has_value() && !altSurfaces->empty())
		{
			for (const auto &surface : *altSurfaces)
			{
				results.push_back(to_hstring(surface));
			}
		}
		return com_array<hstring>(results);
	}

	hstring CelestiaBody::InfoURL()
	{
		return to_hstring(static_cast<Body*>(obj)->getInfoURL());
	}

    CelestiaComponent::CelestiaPlanetarySystem CelestiaBody::System()
    {
        if (p == nullptr)
            p = make<CelestiaPlanetarySystem>(static_cast<Body*>(obj)->getSystem());
        return p;
    }

    CelestiaComponent::CelestiaOrbit CelestiaBody::OrbitAtTime(double julianDay)
    {
        return make<CelestiaOrbit>((celestia::ephem::Orbit*)static_cast<Body*>(obj)->getOrbit(julianDay));
    }

    CelestiaComponent::CelestiaRotationModel CelestiaBody::RotationModelAtTime(double julianDay)
    {
        return make<CelestiaRotationModel>((celestia::ephem::RotationModel*)static_cast<Body*>(obj)->getRotationModel(julianDay));
    }

    bool CelestiaBody::HasRings()
    {
        return GetBodyFeaturesManager()->getRings(static_cast<Body*>(obj));
    }

    bool CelestiaBody::HasAtmosphere()
    {
        return GetBodyFeaturesManager()->getAtmosphere(static_cast<Body*>(obj));
    }

    bool CelestiaBody::IsEllipsoid()
    {
        return static_cast<Body*>(obj)->isEllipsoid();
    }

    float CelestiaBody::Radius()
    {
        return static_cast<Body*>(obj)->getRadius();
    }

    CelestiaComponent::CelestiaTimeline CelestiaBody::Timeline()
    {
        return make<CelestiaTimeline>(static_cast<Body*>(obj)->getTimeline());
    }

    bool CelestiaBody::CanBeUsedAsCockpit()
    {
        return GetBodyFeaturesManager()->canBeUsedAsCockpit(static_cast<Body*>(obj));
    }
}
