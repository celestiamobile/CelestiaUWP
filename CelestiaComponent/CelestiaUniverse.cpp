//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaLocation.h"
#include "CelestiaUniverse.h"
#if __has_include("CelestiaUniverse.g.cpp")
#include "CelestiaUniverse.g.cpp"
#endif

#include <celutil/gettext.h>

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaUniverse::CelestiaUniverse(Universe* u) : CelestiaUniverseT<CelestiaUniverse>(), u(u), starCatalog(nullptr), dsoCatalog(nullptr)
	{
	}

	CelestiaComponent::CelestiaSelection CelestiaUniverse::Find(hstring const& name)
	{
		return make<CelestiaSelection>(u->find(to_string(name)));
	}

	hstring CelestiaUniverse::NameForSelection(CelestiaComponent::CelestiaSelection const& selection)
	{
		Selection s = get_self<CelestiaSelection>(selection)->AsSelection();
		switch (s.getType())
		{
		case Selection::Type_Star:
			return get_self<CelestiaStarCatalog>(StarCatalog())->StarName(make<CelestiaStar>(s.star()));
		case Selection::Type_Body:
			return to_hstring(reinterpret_cast<Body*>(s.object())->getName(true));
		case Selection::Type_DeepSky:
			return get_self<CelestiaDSOCatalog>(DSOCatalog())->DSOName(make<CelestiaDSO>(s.deepsky()));
		case Selection::Type_Location:
			return to_hstring(s.location()->getName(true));
		default:
			return L"";
		}
	}

	bool CelestiaUniverse::IsSelectionMarked(CelestiaComponent::CelestiaSelection const& selection)
	{
		return u->isMarked(get_self<CelestiaSelection>(selection)->AsSelection(), 1);
	}

	void CelestiaUniverse::MarkSelection(CelestiaComponent::CelestiaSelection const& selection, CelestiaComponent::CelestiaMarkerRepresentation marker)
	{
		u->markObject(get_self<CelestiaSelection>(selection)->AsSelection(), celestia::MarkerRepresentation(celestia::MarkerRepresentation::Symbol(marker), 10.0f, Color(0.0f, 1.0f, 0.0f, 0.9f)), 1);
	}

	void CelestiaUniverse::UnmarkSelection(CelestiaComponent::CelestiaSelection const& selection)
	{
		u->unmarkObject(get_self<CelestiaSelection>(selection)->AsSelection(), 1);
	}

	void CelestiaUniverse::UnmarkAll()
	{
		u->unmarkAll();
	}

	com_array<CelestiaComponent::CelestiaBrowserItem> CelestiaUniverse::ChildrenForStar(CelestiaComponent::CelestiaStar const& star, CelestiaComponent::CelestiaBrowserItemChildrenProvider const& provider)
	{
		auto s = reinterpret_cast<Star*>(get_self<CelestiaStar>(star)->obj);
		SolarSystem* ss = u->getSolarSystem(s);
		PlanetarySystem* sys = nullptr;
		if (ss) sys = ss->getPlanets();

		if (!sys)
			return com_array<CelestiaComponent::CelestiaBrowserItem>();

		int sysSize = sys->getSystemSize();
		std::vector<CelestiaComponent::CelestiaBrowserItem> direct;
		std::vector<CelestiaComponent::CelestiaBrowserItem> planets;
		std::vector<CelestiaComponent::CelestiaBrowserItem> dwarfPlanets;
		std::vector<CelestiaComponent::CelestiaBrowserItem> minorMoons;
		std::vector<CelestiaComponent::CelestiaBrowserItem> asteroids;
		std::vector<CelestiaComponent::CelestiaBrowserItem> comets;
		std::vector<CelestiaComponent::CelestiaBrowserItem> spacecraft;

        for (int i = 0; i < sysSize; i++)
        {
            Body* body = sys->getBody(i);
            if (body->getName().empty())
                continue;

            hstring name = to_hstring(body->getName(true));
            if (name.empty())
                continue;

			CelestiaComponent::CelestiaBrowserItem item = make<CelestiaBrowserItem>(name, make<CelestiaBody>(reinterpret_cast<Body*>(body)), provider, false);
			int bodyClass = body->getClassification();
			switch (bodyClass)
			{
			case Body::Invisible:
            case Body::Diffuse:
				continue;
			case Body::Planet:
				planets.push_back(item);
				break;
			case Body::DwarfPlanet:
				dwarfPlanets.push_back(item);
				break;
			case Body::Moon:
			case Body::MinorMoon:
				if (body->getRadius() < 100.0f || Body::MinorMoon == bodyClass)
					minorMoons.push_back(item);
				else
					direct.push_back(item);
				break;
			case Body::Asteroid:
				asteroids.push_back(item);
				break;
			case Body::Comet:
				comets.push_back(item);
				break;
			case Body::Spacecraft:
				spacecraft.push_back(item);
				break;
			default:
				direct.push_back(item);
				break;
			}
        }

		if (spacecraft.size() > 0)
			direct.insert(direct.begin(), make<CelestiaBrowserItem>(to_hstring(_("Spacecraft")), array_view<CelestiaComponent::CelestiaBrowserItem>(spacecraft), false));
		if (comets.size() > 0)
			direct.insert(direct.begin(), make<CelestiaBrowserItem>(to_hstring(_("Comets")), array_view<CelestiaComponent::CelestiaBrowserItem>(comets), false));
		if (asteroids.size() > 0)
			direct.insert(direct.begin(), make<CelestiaBrowserItem>(to_hstring(_("Asteroids")), array_view<CelestiaComponent::CelestiaBrowserItem>(asteroids), false));
		if (minorMoons.size() > 0)
			direct.insert(direct.begin(), make<CelestiaBrowserItem>(to_hstring(_("Minor Moons")), array_view<CelestiaComponent::CelestiaBrowserItem>(minorMoons), false));
		if (dwarfPlanets.size() > 0)
			direct.insert(direct.begin(), make<CelestiaBrowserItem>(to_hstring(_("Dwarf Planets")), array_view<CelestiaComponent::CelestiaBrowserItem>(dwarfPlanets), false));
        if (planets.size() > 0)
            direct.insert(direct.begin(), make<CelestiaBrowserItem>(to_hstring(_("Planets")), array_view<CelestiaComponent::CelestiaBrowserItem>(planets), false));

		return com_array<CelestiaComponent::CelestiaBrowserItem>(direct);
	}

	com_array<CelestiaComponent::CelestiaBrowserItem> CelestiaUniverse::ChildrenForBody(CelestiaComponent::CelestiaBody const& body, CelestiaComponent::CelestiaBrowserItemChildrenProvider const& provider)
	{
		auto b = reinterpret_cast<Body*>(get_self<CelestiaBody>(body)->obj);
		PlanetarySystem* sys = b->getSatellites();

		if (!sys)
			return com_array<CelestiaComponent::CelestiaBrowserItem>();

		int sysSize = sys->getSystemSize();
		std::vector<CelestiaComponent::CelestiaBrowserItem> direct;
		std::vector<CelestiaComponent::CelestiaBrowserItem> minorMoons;
		std::vector<CelestiaComponent::CelestiaBrowserItem> comets;
		std::vector<CelestiaComponent::CelestiaBrowserItem> spacecraft;

		for (int i = 0; i < sysSize; i++)
		{
			Body* body = sys->getBody(i);
			if (body->getName().empty())
				continue;

			hstring name = to_hstring(body->getName(true));
			if (name.empty())
				continue;

			CelestiaComponent::CelestiaBrowserItem item = make<CelestiaBrowserItem>(name, make<CelestiaBody>(reinterpret_cast<Body*>(body)), provider, false);
			int bodyClass = body->getClassification();
			if (bodyClass == Body::Asteroid) bodyClass = Body::Moon;
			switch (bodyClass)
			{
			case Body::Invisible:
            case Body::Diffuse:
				continue;
			case Body::Moon:
			case Body::MinorMoon:
				if (body->getRadius() < 100.0f || Body::MinorMoon == bodyClass)
					minorMoons.push_back(item);
				else
					direct.push_back(item);
				break;
			case Body::Comet:
				comets.push_back(item);
				break;
			case Body::Spacecraft:
				spacecraft.push_back(item);
				break;
			default:
				direct.push_back(item);
				break;
			}
		}

		std::vector<CelestiaComponent::CelestiaBrowserItem> locs;
		std::vector<Location*>* locations = b->getLocations();
		if (locations != nullptr)
		{
			for (std::vector<Location*>::const_iterator iter = locations->begin();
				iter != locations->end(); iter++)
			{
				hstring name = to_hstring((*iter)->getName(true));
				if (name.empty())
					continue;

				auto item = make<CelestiaBrowserItem>(name, make<CelestiaLocation>(*iter), provider, false);
				locs.push_back(item);
			}
			if (locs.size() > 0)
				direct.insert(direct.begin(), make<CelestiaBrowserItem>(to_hstring(_("Locations")), array_view<CelestiaComponent::CelestiaBrowserItem>(locs), false));
		}

		if (spacecraft.size() > 0)
			direct.insert(direct.begin(), make<CelestiaBrowserItem>(to_hstring(_("Spacecraft")), array_view<CelestiaComponent::CelestiaBrowserItem>(spacecraft), false));
		if (comets.size() > 0)
			direct.insert(direct.begin(), make<CelestiaBrowserItem>(to_hstring(_("Comets")), array_view<CelestiaComponent::CelestiaBrowserItem>(comets), false));
		if (minorMoons.size() > 0)
			direct.insert(direct.begin(), make<CelestiaBrowserItem>(to_hstring(_("Minor Moons")), array_view<CelestiaComponent::CelestiaBrowserItem>(minorMoons), false));

		return com_array<CelestiaComponent::CelestiaBrowserItem>(direct);
	}

	CelestiaComponent::CelestiaStarCatalog CelestiaUniverse::StarCatalog()
	{
		if (starCatalog == nullptr)
			starCatalog = make<CelestiaStarCatalog>(u->getStarCatalog());
		return starCatalog;
	}

	CelestiaComponent::CelestiaDSOCatalog CelestiaUniverse::DSOCatalog()
	{
		if (dsoCatalog == nullptr)
			dsoCatalog = make<CelestiaDSOCatalog>(u->getDSOCatalog());
		return dsoCatalog;
	}
}
