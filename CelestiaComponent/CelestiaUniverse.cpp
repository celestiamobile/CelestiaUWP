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

#include <celengine/location.h>
#include <celutil/gettext.h>

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaUniverse::CelestiaUniverse(Universe* u) : CelestiaUniverseT<CelestiaUniverse>(), u(u), starCatalog(nullptr), dsoCatalog(nullptr)
	{
	}

	hstring CelestiaUniverse::NameForSelection(CelestiaComponent::CelestiaSelection const& selection)
	{
		Selection s = get_self<CelestiaSelection>(selection)->AsSelection();
		switch (s.getType())
		{
		case SelectionType::Star:
			return get_self<CelestiaStarCatalog>(StarCatalog())->StarName(make<CelestiaStar>(s.star()));
		case SelectionType::Body:
			return to_hstring(s.body()->getName(true));
		case SelectionType::DeepSky:
			return get_self<CelestiaDSOCatalog>(DSOCatalog())->DSOName(make<CelestiaDSO>(s.deepsky()));
		case SelectionType::Location:
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
			auto bodyClass = body->getClassification();
			switch (bodyClass)
			{
			case BodyClassification::Invisible:
            case BodyClassification::Diffuse:
            case BodyClassification::Component:
				continue;
			case BodyClassification::Planet:
				planets.push_back(item);
				break;
			case BodyClassification::DwarfPlanet:
				dwarfPlanets.push_back(item);
				break;
			case BodyClassification::Moon:
			case BodyClassification::MinorMoon:
				if (body->getRadius() < 100.0f || BodyClassification::MinorMoon == bodyClass)
					minorMoons.push_back(item);
				else
					direct.push_back(item);
				break;
			case BodyClassification::Asteroid:
				asteroids.push_back(item);
				break;
			case BodyClassification::Comet:
				comets.push_back(item);
				break;
			case BodyClassification::Spacecraft:
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
			auto bodyClass = body->getClassification();
			if (bodyClass == BodyClassification::Asteroid) bodyClass = BodyClassification::Moon;
			switch (bodyClass)
			{
			case BodyClassification::Invisible:
            case BodyClassification::Diffuse:
            case BodyClassification::Component:
				continue;
			case BodyClassification::Moon:
			case BodyClassification::MinorMoon:
				if (body->getRadius() < 100.0f || BodyClassification::MinorMoon == bodyClass)
					minorMoons.push_back(item);
				else
					direct.push_back(item);
				break;
			case BodyClassification::Comet:
				comets.push_back(item);
				break;
			case BodyClassification::Spacecraft:
				spacecraft.push_back(item);
				break;
			default:
				direct.push_back(item);
				break;
			}
		}

		std::vector<CelestiaComponent::CelestiaBrowserItem> locs;
		auto locations = GetBodyFeaturesManager()->getLocations(b);
		if (locations.has_value() && !locations->empty())
		{
			for (const auto loc : *locations)
			{
				hstring name = to_hstring(loc->getName(true));
				if (name.empty())
					continue;

				auto item = make<CelestiaBrowserItem>(name, make<CelestiaLocation>(loc), provider, false);
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

    CelestiaComponent::CelestiaStarBrowser CelestiaUniverse::StarBrowser(CelestiaComponent::CelestiaStarBrowserType type, CelestiaComponent::CelestiaObserver const& observer)
    {
        auto b = new celestia::engine::StarBrowser(u);
        auto o = get_self<CelestiaObserver>(observer)->o;
        switch (type)
        {
        case CelestiaComponent::CelestiaStarBrowserType::Nearest:
            b->setComparison(celestia::engine::StarBrowser::Comparison::Nearest);
            b->setFilter(celestia::engine::StarBrowser::Filter::Visible);
            break;
        case CelestiaComponent::CelestiaStarBrowserType::Brighter:
            b->setComparison(celestia::engine::StarBrowser::Comparison::ApparentMagnitude);
            b->setFilter(celestia::engine::StarBrowser::Filter::Visible);
            break;
        case CelestiaComponent::CelestiaStarBrowserType::Brightest:
            b->setComparison(celestia::engine::StarBrowser::Comparison::AbsoluteMagnitude);
            b->setFilter(celestia::engine::StarBrowser::Filter::Visible);
            break;
        case CelestiaComponent::CelestiaStarBrowserType::WithPlants:
            b->setComparison(celestia::engine::StarBrowser::Comparison::Nearest);
            b->setFilter(celestia::engine::StarBrowser::Filter::WithPlanets);
            break;
        default:
            break;
        }
        b->setPosition(o->getPosition());
        b->setTime(o->getTime());
        return make<CelestiaStarBrowser>(b);
    }
}
