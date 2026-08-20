// CelestiaExtension.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaExtension.h"

#include <unordered_map>
#if __has_include("CelestiaExtension.g.cpp")
#include "CelestiaExtension.g.cpp"
#endif

using namespace std;
using namespace winrt;
using namespace Windows::System;

namespace winrt::CelestiaComponent::implementation
{
    com_array<CelestiaBrowserItem> CelestiaExtension::GetChildren(CelestiaBrowserItem const& item, CelestiaAppCore const& appCore)
    {
        auto obj = item.Object();
        if (obj == nullptr)
            return {};
        auto star = obj.try_as<CelestiaStar>();
        if (star != nullptr)
        {
            auto children = appCore.Simulation().Universe().ChildrenForStar(star, [appCore](CelestiaBrowserItem const& item)
                {
                    return GetChildren(item, appCore);
                });
            return children;
        }
        auto body = obj.try_as<CelestiaBody>();
        if (body != nullptr)
        {
            auto children = appCore.Simulation().Universe().ChildrenForBody(body, [appCore](CelestiaBrowserItem const& item)
                {
                    return GetChildren(item, appCore);
                });
            return children;
        }
        return {};
    }

    CelestiaComponent::CelestiaSettingBooleanEntry CelestiaExtension::GetBooleanEntryByName(hstring const& name)
    {
        using Entry = CelestiaComponent::CelestiaSettingBooleanEntry;
        static const std::unordered_map<winrt::hstring, Entry> entries = []
            {
                std::unordered_map<winrt::hstring, Entry> result;
                for (auto value = static_cast<int>(Entry::ShowStars); value <= static_cast<int>(Entry::SeparateRayleighMieScaleHeights); ++value)
                {
                    auto entry = static_cast<Entry>(value);
                    auto entryName = GetNameByBooleanEntry(entry);
                    if (!entryName.empty())
                        result.emplace(entryName, entry);
                }
                return result;
            }();

        auto iterator = entries.find(name);
        return iterator == entries.end() ? Entry::None : iterator->second;
    }

    CelestiaComponent::CelestiaSettingInt32Entry CelestiaExtension::GetInt32EntryByName(hstring const& name)
    {
        using Entry = CelestiaComponent::CelestiaSettingInt32Entry;
        static const std::unordered_map<winrt::hstring, Entry> entries = []
            {
                std::unordered_map<winrt::hstring, Entry> result;
                for (auto value = static_cast<int>(Entry::Resolution); value <= static_cast<int>(Entry::CloudSegmentCount); ++value)
                {
                    auto entry = static_cast<Entry>(value);
                    auto entryName = GetNameByInt32Entry(entry);
                    if (!entryName.empty())
                        result.emplace(entryName, entry);
                }
                return result;
            }();

        auto iterator = entries.find(name);
        return iterator == entries.end() ? Entry::None : iterator->second;
    }

    CelestiaComponent::CelestiaSettingSingleEntry CelestiaExtension::GetSingleEntryByName(hstring const& name)
    {
        using Entry = CelestiaComponent::CelestiaSettingSingleEntry;
        static const std::unordered_map<winrt::hstring, Entry> entries = []
            {
                std::unordered_map<winrt::hstring, Entry> result;
                for (auto value = static_cast<int>(Entry::AmbientLightLevel); value <= static_cast<int>(Entry::Exposure); ++value)
                {
                    auto entry = static_cast<Entry>(value);
                    auto entryName = GetNameBySingleEntry(entry);
                    if (!entryName.empty())
                        result.emplace(entryName, entry);
                }
                return result;
            }();

        auto iterator = entries.find(name);
        return iterator == entries.end() ? Entry::None : iterator->second;
    }

    hstring CelestiaExtension::GetNameByBooleanEntry(CelestiaComponent::CelestiaSettingBooleanEntry entry)
    {
        switch (entry)
        {
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowStars:
            return L"ShowStars";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanets:
            return L"ShowPlanets";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanets:
            return L"ShowDwarfPlanets";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoons:
            return L"ShowMoons";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoons:
            return L"ShowMinorMoons";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroids:
            return L"ShowAsteroids";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowComets:
            return L"ShowComets";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecrafts:
            return L"ShowSpacecrafts";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxies:
            return L"ShowGalaxies";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobulars:
            return L"ShowGlobulars";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulae:
            return L"ShowNebulae";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusters:
            return L"ShowOpenClusters";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDiagrams:
            return L"ShowDiagrams";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowBoundaries:
            return L"ShowBoundaries";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudMaps:
            return L"ShowCloudMaps";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowNightMaps:
            return L"ShowNightMaps";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAtmospheres:
            return L"ShowAtmospheres";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometTails:
            return L"ShowCometTails";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetRings:
            return L"ShowPlanetRings";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMarkers:
            return L"ShowMarkers";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOrbits:
            return L"ShowOrbits";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFadingOrbits:
            return L"ShowFadingOrbits";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEcliptic:
            return L"ShowEcliptic";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPartialTrajectories:
            return L"ShowPartialTrajectories";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSmoothLines:
            return L"ShowSmoothLines";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipseShadows:
            return L"ShowEclipseShadows";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRingShadows:
            return L"ShowRingShadows";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudShadows:
            return L"ShowCloudShadows";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAutoMag:
            return L"ShowAutoMag";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCelestialSphere:
            return L"ShowCelestialSphere";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipticGrid:
            return L"ShowEclipticGrid";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowHorizonGrid:
            return L"ShowHorizonGrid";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalacticGrid:
            return L"ShowGalacticGrid";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowStarLabels:
            return L"ShowStarLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetLabels:
            return L"ShowPlanetLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonLabels:
            return L"ShowMoonLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowConstellationLabels:
            return L"ShowConstellationLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxyLabels:
            return L"ShowGalaxyLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobularLabels:
            return L"ShowGlobularLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulaLabels:
            return L"ShowNebulaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusterLabels:
            return L"ShowOpenClusterLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidLabels:
            return L"ShowAsteroidLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftLabels:
            return L"ShowSpacecraftLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLocationLabels:
            return L"ShowLocationLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometLabels:
            return L"ShowCometLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetLabels:
            return L"ShowDwarfPlanetLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonLabels:
            return L"ShowMinorMoonLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLatinConstellationLabels:
            return L"ShowLatinConstellationLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetOrbits:
            return L"ShowPlanetOrbits";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonOrbits:
            return L"ShowMoonOrbits";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidOrbits:
            return L"ShowAsteroidOrbits";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftOrbits:
            return L"ShowSpacecraftOrbits";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometOrbits:
            return L"ShowCometOrbits";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowStellarOrbits:
            return L"ShowStellarOrbits";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetOrbits:
            return L"ShowDwarfPlanetOrbits";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonOrbits:
            return L"ShowMinorMoonOrbits";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCityLabels:
            return L"ShowCityLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowObservatoryLabels:
            return L"ShowObservatoryLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLandingSiteLabels:
            return L"ShowLandingSiteLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCraterLabels:
            return L"ShowCraterLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowVallisLabels:
            return L"ShowVallisLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMonsLabels:
            return L"ShowMonsLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanumLabels:
            return L"ShowPlanumLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowChasmaLabels:
            return L"ShowChasmaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCollumLabels:
            return L"ShowCollumLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPateraLabels:
            return L"ShowPateraLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMareLabels:
            return L"ShowMareLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRupesLabels:
            return L"ShowRupesLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowTesseraLabels:
            return L"ShowTesseraLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRegioLabels:
            return L"ShowRegioLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowChaosLabels:
            return L"ShowChaosLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowTerraLabels:
            return L"ShowTerraLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAstrumLabels:
            return L"ShowAstrumLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCoronaLabels:
            return L"ShowCoronaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDorsumLabels:
            return L"ShowDorsumLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFossaLabels:
            return L"ShowFossaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCatenaLabels:
            return L"ShowCatenaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMensaLabels:
            return L"ShowMensaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRimaLabels:
            return L"ShowRimaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowUndaeLabels:
            return L"ShowUndaeLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowReticulumLabels:
            return L"ShowReticulumLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanitiaLabels:
            return L"ShowPlanitiaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLineaLabels:
            return L"ShowLineaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFluctusLabels:
            return L"ShowFluctusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFarrumLabels:
            return L"ShowFarrumLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEruptiveCenterLabels:
            return L"ShowEruptiveCenterLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowTholusLabels:
            return L"ShowTholusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowInsulaLabels:
            return L"ShowInsulaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAlbedoLabels:
            return L"ShowAlbedoLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowArcusLabels:
            return L"ShowArcusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCavusLabels:
            return L"ShowCavusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCollesLabels:
            return L"ShowCollesLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFaculaLabels:
            return L"ShowFaculaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFlexusLabels:
            return L"ShowFlexusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFlumenLabels:
            return L"ShowFlumenLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFretumLabels:
            return L"ShowFretumLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLabesLabels:
            return L"ShowLabesLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLabyrinthusLabels:
            return L"ShowLabyrinthusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLacunaLabels:
            return L"ShowLacunaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLacusLabels:
            return L"ShowLacusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLargeRingedLabels:
            return L"ShowLargeRingedLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLobusLabels:
            return L"ShowLobusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLingulaLabels:
            return L"ShowLingulaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMaculaLabels:
            return L"ShowMaculaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOceanusLabels:
            return L"ShowOceanusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPalusLabels:
            return L"ShowPalusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlumeLabels:
            return L"ShowPlumeLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPromontoriumLabels:
            return L"ShowPromontoriumLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSatelliteLabels:
            return L"ShowSatelliteLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowScopulusLabels:
            return L"ShowScopulusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSerpensLabels:
            return L"ShowSerpensLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSinusLabels:
            return L"ShowSinusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSulcusLabels:
            return L"ShowSulcusLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowVastitasLabels:
            return L"ShowVastitasLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowVirgaLabels:
            return L"ShowVirgaLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSaxumLabels:
            return L"ShowSaxumLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCosmodromeLabels:
            return L"ShowCosmodromeLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRingLabels:
            return L"ShowRingLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOtherLabels:
            return L"ShowOtherLabels";
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableReverseWheel:
            return L"EnableReverseWheel";
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableRayBasedDragging:
            return L"EnableRayBasedDragging";
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableFocusZooming:
            return L"EnableFocusZooming";
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableAlignCameraToSurfaceOnLand:
            return L"EnableAlignCameraToSurfaceOnLand";
        case CelestiaComponent::CelestiaSettingBooleanEntry::SeparateRayleighMieScaleHeights:
            return L"SeparateRayleighMieScaleHeights";
        default:
            break;
        }
        return L"";
    }

    hstring CelestiaExtension::GetNameByInt32Entry(CelestiaComponent::CelestiaSettingInt32Entry entry)
    {
        switch (entry)
        {
        case CelestiaComponent::CelestiaSettingInt32Entry::Resolution:
            return L"Resolution";
        case CelestiaComponent::CelestiaSettingInt32Entry::StarStyle:
            return L"StarStyle";
        case CelestiaComponent::CelestiaSettingInt32Entry::HudDetail:
            return L"HudDetail";
        case CelestiaComponent::CelestiaSettingInt32Entry::DateFormat:
            return L"DateFormat";
        case CelestiaComponent::CelestiaSettingInt32Entry::TimeZone:
            return L"TimeZone";
        case CelestiaComponent::CelestiaSettingInt32Entry::MeasurementSystem:
            return L"MeasurementSystem";
        case CelestiaComponent::CelestiaSettingInt32Entry::TemperatureScale:
            return L"TemperatureScale";
        case CelestiaComponent::CelestiaSettingInt32Entry::ScriptSystemAccessPolicy:
            return L"ScriptSystemAccessPolicy";
        case CelestiaComponent::CelestiaSettingInt32Entry::StarColors:
            return L"StarColors";
        case CelestiaComponent::CelestiaSettingInt32Entry::ToneMapping:
            return L"ToneMapping";
        case CelestiaComponent::CelestiaSettingInt32Entry::AtmosphereSegmentCount:
            return L"AtmosphereSegmentCount";
        case CelestiaComponent::CelestiaSettingInt32Entry::CloudSegmentCount:
            return L"CloudSegmentCount";
        default:
            break;
        }
        return L"";
    }

    hstring CelestiaExtension::GetNameBySingleEntry(CelestiaComponent::CelestiaSettingSingleEntry entry)
    {
        switch (entry)
        {
        case CelestiaComponent::CelestiaSettingSingleEntry::AmbientLightLevel:
            return L"AmbientLightLevel";
        case CelestiaComponent::CelestiaSettingSingleEntry::FaintestVisible:
            return L"FaintestVisible";
        case CelestiaComponent::CelestiaSettingSingleEntry::GalaxyBrightness:
            return L"GalaxyBrightness";
        case CelestiaComponent::CelestiaSettingSingleEntry::MinimumFeatureSize:
            return L"MinimumFeatureSize";
        case CelestiaComponent::CelestiaSettingSingleEntry::DistanceLimit:
            return L"DistanceLimit";
        case CelestiaComponent::CelestiaSettingSingleEntry::TintSaturation:
            return L"TintSaturation";
        case CelestiaComponent::CelestiaSettingSingleEntry::StarPointRadius:
            return L"StarPointRadius";
        case CelestiaComponent::CelestiaSettingSingleEntry::StarOptimization:
            return L"StarOptimization";
        case CelestiaComponent::CelestiaSettingSingleEntry::StarMaxIrradiance:
            return L"StarMaxIrradiance";
        case CelestiaComponent::CelestiaSettingSingleEntry::StarDimClipFactor:
            return L"StarDimClipFactor";
        case CelestiaComponent::CelestiaSettingSingleEntry::StarExposure:
            return L"StarExposure";
        case CelestiaComponent::CelestiaSettingSingleEntry::Exposure:
            return L"Exposure";
        default:
            break;
        }
        return L"";
    }

    void CelestiaExtension::SetCelestiaBooleanValue(CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingBooleanEntry entry, bool value)
    {
        switch (entry)
        {
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowStars:
            appCore.ShowStars(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanets:
            appCore.ShowPlanets(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanets:
            appCore.ShowDwarfPlanets(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoons:
            appCore.ShowMoons(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoons:
            appCore.ShowMinorMoons(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroids:
            appCore.ShowAsteroids(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowComets:
            appCore.ShowComets(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecrafts:
            appCore.ShowSpacecrafts(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxies:
            appCore.ShowGalaxies(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobulars:
            appCore.ShowGlobulars(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulae:
            appCore.ShowNebulae(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusters:
            appCore.ShowOpenClusters(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDiagrams:
            appCore.ShowDiagrams(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowBoundaries:
            appCore.ShowBoundaries(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudMaps:
            appCore.ShowCloudMaps(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowNightMaps:
            appCore.ShowNightMaps(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAtmospheres:
            appCore.ShowAtmospheres(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometTails:
            appCore.ShowCometTails(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetRings:
            appCore.ShowPlanetRings(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMarkers:
            appCore.ShowMarkers(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOrbits:
            appCore.ShowOrbits(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFadingOrbits:
            appCore.ShowFadingOrbits(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEcliptic:
            appCore.ShowEcliptic(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPartialTrajectories:
            appCore.ShowPartialTrajectories(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSmoothLines:
            appCore.ShowSmoothLines(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipseShadows:
            appCore.ShowEclipseShadows(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRingShadows:
            appCore.ShowRingShadows(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudShadows:
            appCore.ShowCloudShadows(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAutoMag:
            appCore.ShowAutoMag(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCelestialSphere:
            appCore.ShowCelestialSphere(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipticGrid:
            appCore.ShowEclipticGrid(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowHorizonGrid:
            appCore.ShowHorizonGrid(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalacticGrid:
            appCore.ShowGalacticGrid(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowStarLabels:
            appCore.ShowStarLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetLabels:
            appCore.ShowPlanetLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonLabels:
            appCore.ShowMoonLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowConstellationLabels:
            appCore.ShowConstellationLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxyLabels:
            appCore.ShowGalaxyLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobularLabels:
            appCore.ShowGlobularLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulaLabels:
            appCore.ShowNebulaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusterLabels:
            appCore.ShowOpenClusterLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidLabels:
            appCore.ShowAsteroidLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftLabels:
            appCore.ShowSpacecraftLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLocationLabels:
            appCore.ShowLocationLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometLabels:
            appCore.ShowCometLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetLabels:
            appCore.ShowDwarfPlanetLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonLabels:
            appCore.ShowMinorMoonLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLatinConstellationLabels:
            appCore.ShowLatinConstellationLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetOrbits:
            appCore.ShowPlanetOrbits(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonOrbits:
            appCore.ShowMoonOrbits(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidOrbits:
            appCore.ShowAsteroidOrbits(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftOrbits:
            appCore.ShowSpacecraftOrbits(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometOrbits:
            appCore.ShowCometOrbits(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowStellarOrbits:
            appCore.ShowStellarOrbits(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetOrbits:
            appCore.ShowDwarfPlanetOrbits(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonOrbits:
            appCore.ShowMinorMoonOrbits(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCityLabels:
            appCore.ShowCityLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowObservatoryLabels:
            appCore.ShowObservatoryLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLandingSiteLabels:
            appCore.ShowLandingSiteLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCraterLabels:
            appCore.ShowCraterLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowVallisLabels:
            appCore.ShowVallisLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMonsLabels:
            appCore.ShowMonsLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanumLabels:
            appCore.ShowPlanumLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowChasmaLabels:
            appCore.ShowChasmaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCollumLabels:
            appCore.ShowCollumLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPateraLabels:
            appCore.ShowPateraLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMareLabels:
            appCore.ShowMareLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRupesLabels:
            appCore.ShowRupesLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowTesseraLabels:
            appCore.ShowTesseraLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRegioLabels:
            appCore.ShowRegioLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowChaosLabels:
            appCore.ShowChaosLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowTerraLabels:
            appCore.ShowTerraLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAstrumLabels:
            appCore.ShowAstrumLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCoronaLabels:
            appCore.ShowCoronaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDorsumLabels:
            appCore.ShowDorsumLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFossaLabels:
            appCore.ShowFossaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCatenaLabels:
            appCore.ShowCatenaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMensaLabels:
            appCore.ShowMensaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRimaLabels:
            appCore.ShowRimaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowUndaeLabels:
            appCore.ShowUndaeLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowReticulumLabels:
            appCore.ShowReticulumLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanitiaLabels:
            appCore.ShowPlanitiaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLineaLabels:
            appCore.ShowLineaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFluctusLabels:
            appCore.ShowFluctusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFarrumLabels:
            appCore.ShowFarrumLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEruptiveCenterLabels:
            appCore.ShowEruptiveCenterLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowTholusLabels:
            appCore.ShowTholusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowInsulaLabels:
            appCore.ShowInsulaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAlbedoLabels:
            appCore.ShowAlbedoLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowArcusLabels:
            appCore.ShowArcusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCavusLabels:
            appCore.ShowCavusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCollesLabels:
            appCore.ShowCollesLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFaculaLabels:
            appCore.ShowFaculaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFlexusLabels:
            appCore.ShowFlexusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFlumenLabels:
            appCore.ShowFlumenLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFretumLabels:
            appCore.ShowFretumLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLabesLabels:
            appCore.ShowLabesLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLabyrinthusLabels:
            appCore.ShowLabyrinthusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLacunaLabels:
            appCore.ShowLacunaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLacusLabels:
            appCore.ShowLacusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLargeRingedLabels:
            appCore.ShowLargeRingedLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLobusLabels:
            appCore.ShowLobusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLingulaLabels:
            appCore.ShowLingulaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMaculaLabels:
            appCore.ShowMaculaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOceanusLabels:
            appCore.ShowOceanusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPalusLabels:
            appCore.ShowPalusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlumeLabels:
            appCore.ShowPlumeLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPromontoriumLabels:
            appCore.ShowPromontoriumLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSatelliteLabels:
            appCore.ShowSatelliteLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowScopulusLabels:
            appCore.ShowScopulusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSerpensLabels:
            appCore.ShowSerpensLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSinusLabels:
            appCore.ShowSinusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSulcusLabels:
            appCore.ShowSulcusLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowVastitasLabels:
            appCore.ShowVastitasLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowVirgaLabels:
            appCore.ShowVirgaLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSaxumLabels:
            appCore.ShowSaxumLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCosmodromeLabels:
            appCore.ShowCosmodromeLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRingLabels:
            appCore.ShowRingLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOtherLabels:
            appCore.ShowOtherLabels(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableReverseWheel:
            appCore.EnableReverseWheel(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableRayBasedDragging:
            appCore.EnableRayBasedDragging(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableFocusZooming:
            appCore.EnableFocusZooming(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableAlignCameraToSurfaceOnLand:
            appCore.EnableAlignCameraToSurfaceOnLand(value);
            break;
        case CelestiaComponent::CelestiaSettingBooleanEntry::SeparateRayleighMieScaleHeights:
            appCore.SeparateRayleighMieScaleHeights(value);
            break;
        default:
            break;
        }
    }

    void CelestiaExtension::SetCelestiaInt32Value(CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingInt32Entry entry, int32_t value)
    {
        switch (entry)
        {
        case CelestiaComponent::CelestiaSettingInt32Entry::Resolution:
            appCore.Resolution(value);
            break;
        case CelestiaComponent::CelestiaSettingInt32Entry::StarStyle:
            appCore.StarStyle(value);
            break;
        case CelestiaComponent::CelestiaSettingInt32Entry::HudDetail:
            appCore.HudDetail(value);
            break;
        case CelestiaComponent::CelestiaSettingInt32Entry::DateFormat:
            appCore.DateFormat(value);
            break;
        case CelestiaComponent::CelestiaSettingInt32Entry::TimeZone:
            appCore.TimeZone(value);
            break;
        case CelestiaComponent::CelestiaSettingInt32Entry::MeasurementSystem:
            appCore.MeasurementSystem(value);
            break;
        case CelestiaComponent::CelestiaSettingInt32Entry::TemperatureScale:
            appCore.TemperatureScale(value);
            break;
        case CelestiaComponent::CelestiaSettingInt32Entry::ScriptSystemAccessPolicy:
            appCore.ScriptSystemAccessPolicy(value);
            break;
        case CelestiaComponent::CelestiaSettingInt32Entry::StarColors:
            appCore.StarColors(value);
            break;
        case CelestiaComponent::CelestiaSettingInt32Entry::ToneMapping:
            appCore.ToneMapping(value);
            break;
        case CelestiaComponent::CelestiaSettingInt32Entry::AtmosphereSegmentCount:
            appCore.AtmosphereSegmentCount(value);
            break;
        case CelestiaComponent::CelestiaSettingInt32Entry::CloudSegmentCount:
            appCore.CloudSegmentCount(value);
            break;
        default:
            break;
        }
    }

    void CelestiaExtension::SetCelestiaSingleValue(CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingSingleEntry entry, float value)
    {
        switch (entry)
        {
        case CelestiaComponent::CelestiaSettingSingleEntry::AmbientLightLevel:
            appCore.AmbientLightLevel(value);
            break;
        case CelestiaComponent::CelestiaSettingSingleEntry::FaintestVisible:
            appCore.FaintestVisible(value);
            break;
        case CelestiaComponent::CelestiaSettingSingleEntry::GalaxyBrightness:
            appCore.GalaxyBrightness(value);
            break;
        case CelestiaComponent::CelestiaSettingSingleEntry::MinimumFeatureSize:
            appCore.MinimumFeatureSize(value);
            break;
        case CelestiaComponent::CelestiaSettingSingleEntry::DistanceLimit:
            appCore.DistanceLimit(value);
            break;
        case CelestiaComponent::CelestiaSettingSingleEntry::TintSaturation:
            appCore.TintSaturation(value);
            break;
        case CelestiaComponent::CelestiaSettingSingleEntry::StarPointRadius:
            appCore.StarPointRadius(value);
            break;
        case CelestiaComponent::CelestiaSettingSingleEntry::StarOptimization:
            appCore.StarOptimization(value);
            break;
        case CelestiaComponent::CelestiaSettingSingleEntry::StarMaxIrradiance:
            appCore.StarMaxIrradiance(value);
            break;
        case CelestiaComponent::CelestiaSettingSingleEntry::StarDimClipFactor:
            appCore.StarDimClipFactor(value);
            break;
        case CelestiaComponent::CelestiaSettingSingleEntry::StarExposure:
            appCore.StarExposure(value);
            break;
        case CelestiaComponent::CelestiaSettingSingleEntry::Exposure:
            appCore.Exposure(value);
            break;
        default:
            break;
        }
    }

    bool CelestiaExtension::GetCelestiaBooleanValue(CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingBooleanEntry entry)
    {
        switch (entry)
        {
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowStars:
            return appCore.ShowStars();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanets:
            return appCore.ShowPlanets();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanets:
            return appCore.ShowDwarfPlanets();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoons:
            return appCore.ShowMoons();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoons:
            return appCore.ShowMinorMoons();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroids:
            return appCore.ShowAsteroids();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowComets:
            return appCore.ShowComets();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecrafts:
            return appCore.ShowSpacecrafts();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxies:
            return appCore.ShowGalaxies();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobulars:
            return appCore.ShowGlobulars();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulae:
            return appCore.ShowNebulae();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusters:
            return appCore.ShowOpenClusters();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDiagrams:
            return appCore.ShowDiagrams();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowBoundaries:
            return appCore.ShowBoundaries();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudMaps:
            return appCore.ShowCloudMaps();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowNightMaps:
            return appCore.ShowNightMaps();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAtmospheres:
            return appCore.ShowAtmospheres();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometTails:
            return appCore.ShowCometTails();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetRings:
            return appCore.ShowPlanetRings();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMarkers:
            return appCore.ShowMarkers();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOrbits:
            return appCore.ShowOrbits();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFadingOrbits:
            return appCore.ShowFadingOrbits();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEcliptic:
            return appCore.ShowEcliptic();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPartialTrajectories:
            return appCore.ShowPartialTrajectories();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSmoothLines:
            return appCore.ShowSmoothLines();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipseShadows:
            return appCore.ShowEclipseShadows();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRingShadows:
            return appCore.ShowRingShadows();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudShadows:
            return appCore.ShowCloudShadows();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAutoMag:
            return appCore.ShowAutoMag();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCelestialSphere:
            return appCore.ShowCelestialSphere();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipticGrid:
            return appCore.ShowEclipticGrid();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowHorizonGrid:
            return appCore.ShowHorizonGrid();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalacticGrid:
            return appCore.ShowGalacticGrid();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowStarLabels:
            return appCore.ShowStarLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetLabels:
            return appCore.ShowPlanetLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonLabels:
            return appCore.ShowMoonLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowConstellationLabels:
            return appCore.ShowConstellationLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxyLabels:
            return appCore.ShowGalaxyLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobularLabels:
            return appCore.ShowGlobularLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulaLabels:
            return appCore.ShowNebulaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusterLabels:
            return appCore.ShowOpenClusterLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidLabels:
            return appCore.ShowAsteroidLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftLabels:
            return appCore.ShowSpacecraftLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLocationLabels:
            return appCore.ShowLocationLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometLabels:
            return appCore.ShowCometLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetLabels:
            return appCore.ShowDwarfPlanetLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonLabels:
            return appCore.ShowMinorMoonLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLatinConstellationLabels:
            return appCore.ShowLatinConstellationLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetOrbits:
            return appCore.ShowPlanetOrbits();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonOrbits:
            return appCore.ShowMoonOrbits();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidOrbits:
            return appCore.ShowAsteroidOrbits();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftOrbits:
            return appCore.ShowSpacecraftOrbits();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometOrbits:
            return appCore.ShowCometOrbits();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowStellarOrbits:
            return appCore.ShowStellarOrbits();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetOrbits:
            return appCore.ShowDwarfPlanetOrbits();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonOrbits:
            return appCore.ShowMinorMoonOrbits();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCityLabels:
            return appCore.ShowCityLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowObservatoryLabels:
            return appCore.ShowObservatoryLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLandingSiteLabels:
            return appCore.ShowLandingSiteLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCraterLabels:
            return appCore.ShowCraterLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowVallisLabels:
            return appCore.ShowVallisLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMonsLabels:
            return appCore.ShowMonsLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanumLabels:
            return appCore.ShowPlanumLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowChasmaLabels:
            return appCore.ShowChasmaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCollumLabels:
            return appCore.ShowCollumLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPateraLabels:
            return appCore.ShowPateraLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMareLabels:
            return appCore.ShowMareLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRupesLabels:
            return appCore.ShowRupesLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowTesseraLabels:
            return appCore.ShowTesseraLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRegioLabels:
            return appCore.ShowRegioLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowChaosLabels:
            return appCore.ShowChaosLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowTerraLabels:
            return appCore.ShowTerraLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAstrumLabels:
            return appCore.ShowAstrumLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCoronaLabels:
            return appCore.ShowCoronaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowDorsumLabels:
            return appCore.ShowDorsumLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFossaLabels:
            return appCore.ShowFossaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCatenaLabels:
            return appCore.ShowCatenaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMensaLabels:
            return appCore.ShowMensaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRimaLabels:
            return appCore.ShowRimaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowUndaeLabels:
            return appCore.ShowUndaeLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowReticulumLabels:
            return appCore.ShowReticulumLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanitiaLabels:
            return appCore.ShowPlanitiaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLineaLabels:
            return appCore.ShowLineaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFluctusLabels:
            return appCore.ShowFluctusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFarrumLabels:
            return appCore.ShowFarrumLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowEruptiveCenterLabels:
            return appCore.ShowEruptiveCenterLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowTholusLabels:
            return appCore.ShowTholusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowInsulaLabels:
            return appCore.ShowInsulaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowAlbedoLabels:
            return appCore.ShowAlbedoLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowArcusLabels:
            return appCore.ShowArcusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCavusLabels:
            return appCore.ShowCavusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCollesLabels:
            return appCore.ShowCollesLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFaculaLabels:
            return appCore.ShowFaculaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFlexusLabels:
            return appCore.ShowFlexusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFlumenLabels:
            return appCore.ShowFlumenLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowFretumLabels:
            return appCore.ShowFretumLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLabesLabels:
            return appCore.ShowLabesLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLabyrinthusLabels:
            return appCore.ShowLabyrinthusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLacunaLabels:
            return appCore.ShowLacunaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLacusLabels:
            return appCore.ShowLacusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLargeRingedLabels:
            return appCore.ShowLargeRingedLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLobusLabels:
            return appCore.ShowLobusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowLingulaLabels:
            return appCore.ShowLingulaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowMaculaLabels:
            return appCore.ShowMaculaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOceanusLabels:
            return appCore.ShowOceanusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPalusLabels:
            return appCore.ShowPalusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlumeLabels:
            return appCore.ShowPlumeLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowPromontoriumLabels:
            return appCore.ShowPromontoriumLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSatelliteLabels:
            return appCore.ShowSatelliteLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowScopulusLabels:
            return appCore.ShowScopulusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSerpensLabels:
            return appCore.ShowSerpensLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSinusLabels:
            return appCore.ShowSinusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSulcusLabels:
            return appCore.ShowSulcusLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowVastitasLabels:
            return appCore.ShowVastitasLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowVirgaLabels:
            return appCore.ShowVirgaLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowSaxumLabels:
            return appCore.ShowSaxumLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowCosmodromeLabels:
            return appCore.ShowCosmodromeLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowRingLabels:
            return appCore.ShowRingLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOtherLabels:
            return appCore.ShowOtherLabels();
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableReverseWheel:
            return appCore.EnableReverseWheel();
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableRayBasedDragging:
            return appCore.EnableRayBasedDragging();
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableFocusZooming:
            return appCore.EnableFocusZooming();
        case CelestiaComponent::CelestiaSettingBooleanEntry::EnableAlignCameraToSurfaceOnLand:
            return appCore.EnableAlignCameraToSurfaceOnLand();
        case CelestiaComponent::CelestiaSettingBooleanEntry::SeparateRayleighMieScaleHeights:
            return appCore.SeparateRayleighMieScaleHeights();
        default:
            break;
        }
        return false;
    }

    int32_t CelestiaExtension::GetCelestiaInt32Value(CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingInt32Entry entry)
    {
        switch (entry)
        {
        case CelestiaComponent::CelestiaSettingInt32Entry::Resolution:
            return appCore.Resolution();
        case CelestiaComponent::CelestiaSettingInt32Entry::StarStyle:
            return appCore.StarStyle();
        case CelestiaComponent::CelestiaSettingInt32Entry::HudDetail:
            return appCore.HudDetail();
        case CelestiaComponent::CelestiaSettingInt32Entry::DateFormat:
            return appCore.DateFormat();
        case CelestiaComponent::CelestiaSettingInt32Entry::TimeZone:
            return appCore.TimeZone();
        case CelestiaComponent::CelestiaSettingInt32Entry::MeasurementSystem:
            return appCore.MeasurementSystem();
        case CelestiaComponent::CelestiaSettingInt32Entry::TemperatureScale:
            return appCore.TemperatureScale();
        case CelestiaComponent::CelestiaSettingInt32Entry::ScriptSystemAccessPolicy:
            return appCore.ScriptSystemAccessPolicy();
        case CelestiaComponent::CelestiaSettingInt32Entry::StarColors:
            return appCore.StarColors();
        case CelestiaComponent::CelestiaSettingInt32Entry::ToneMapping:
            return appCore.ToneMapping();
        case CelestiaComponent::CelestiaSettingInt32Entry::AtmosphereSegmentCount:
            return appCore.AtmosphereSegmentCount();
        case CelestiaComponent::CelestiaSettingInt32Entry::CloudSegmentCount:
            return appCore.CloudSegmentCount();
        default:
            break;
        }
        return 0;
    }

    float CelestiaExtension::GetCelestiaSingleValue(CelestiaAppCore const& appCore, CelestiaComponent::CelestiaSettingSingleEntry entry)
    {
        switch (entry)
        {
        case CelestiaComponent::CelestiaSettingSingleEntry::AmbientLightLevel:
            return appCore.AmbientLightLevel();
        case CelestiaComponent::CelestiaSettingSingleEntry::FaintestVisible:
            return appCore.FaintestVisible();
        case CelestiaComponent::CelestiaSettingSingleEntry::GalaxyBrightness:
            return appCore.GalaxyBrightness();
        case CelestiaComponent::CelestiaSettingSingleEntry::MinimumFeatureSize:
            return appCore.MinimumFeatureSize();
        case CelestiaComponent::CelestiaSettingSingleEntry::DistanceLimit:
            return appCore.DistanceLimit();
        case CelestiaComponent::CelestiaSettingSingleEntry::TintSaturation:
            return appCore.TintSaturation();
        case CelestiaComponent::CelestiaSettingSingleEntry::StarPointRadius:
            return appCore.StarPointRadius();
        case CelestiaComponent::CelestiaSettingSingleEntry::StarOptimization:
            return appCore.StarOptimization();
        case CelestiaComponent::CelestiaSettingSingleEntry::StarMaxIrradiance:
            return appCore.StarMaxIrradiance();
        case CelestiaComponent::CelestiaSettingSingleEntry::StarDimClipFactor:
            return appCore.StarDimClipFactor();
        case CelestiaComponent::CelestiaSettingSingleEntry::StarExposure:
            return appCore.StarExposure();
        case CelestiaComponent::CelestiaSettingSingleEntry::Exposure:
            return appCore.Exposure();
        default:
            break;
        }
        return 0.0f;
    }

    void CelestiaExtension::InvokeCelestiaGamepadAction(CelestiaComponent::CelestiaAppCore const& appCore, CelestiaComponent::CelestiaGamepadAction action, bool up)
    {
        switch (action)
        {
        case CelestiaGamepadAction::MoveFaster:
            up ? appCore.JoystickButtonUp(CelestiaJoystickButton::Button2) : appCore.JoystickButtonDown(CelestiaJoystickButton::Button2);
            break;
        case CelestiaGamepadAction::MoveSlower:
            up ? appCore.JoystickButtonUp(CelestiaJoystickButton::Button1) : appCore.JoystickButtonDown(CelestiaJoystickButton::Button1);
            break;
        case CelestiaGamepadAction::StopSpeed:
            if (up)
                appCore.Perform(CelestiaAction::Stop);
            break;
        case CelestiaGamepadAction::ReverseSpeed:
            if (up)
                appCore.Perform(CelestiaAction::ReverseSpeed);
            break;
        case CelestiaGamepadAction::ReverseOrientation:
            if (up)
                appCore.Simulation().ReverseOrientation();
            break;
        case CelestiaGamepadAction::TapCenter:
            {
                auto x = static_cast<float>(appCore.GetWidth()) / 2.0f;
                auto y = static_cast<float>(appCore.GetHeight()) / 2.0f;
                up ? appCore.MouseButtonUp(x, y, CelestiaMouseButton::Left) : appCore.MouseButtonDown(x, y, CelestiaMouseButton::Left);
            }
            break;
        case CelestiaGamepadAction::GoTo:
            if (up)
                appCore.Perform(CelestiaAction::GoTo);
            break;
        case CelestiaGamepadAction::Esc:
            if (up)
                appCore.Perform(CelestiaAction::CancelScript);
            break;
        case CelestiaGamepadAction::PitchUp:
            up ? appCore.KeyUp(static_cast<int32_t>(VirtualKey::NumberPad2), 0) : appCore.KeyDown(static_cast<int32_t>(VirtualKey::NumberPad2), 0);
            break;
        case CelestiaGamepadAction::PitchDown:
            up ? appCore.KeyUp(static_cast<int32_t>(VirtualKey::NumberPad8), 0) : appCore.KeyDown(static_cast<int32_t>(VirtualKey::NumberPad8), 0);
            break;
        case CelestiaGamepadAction::YawLeft:
            up ? appCore.KeyUp(static_cast<int32_t>(VirtualKey::NumberPad4), 0) : appCore.KeyDown(static_cast<int32_t>(VirtualKey::NumberPad4), 0);
            break;
        case CelestiaGamepadAction::YawRight:
            up ? appCore.KeyUp(static_cast<int32_t>(VirtualKey::NumberPad6), 0) : appCore.KeyDown(static_cast<int32_t>(VirtualKey::NumberPad6), 0);
            break;
        case CelestiaGamepadAction::RollLeft:
            up ? appCore.KeyUp(static_cast<int32_t>(VirtualKey::NumberPad7), 0) : appCore.KeyDown(static_cast<int32_t>(VirtualKey::NumberPad7), 0);
            break;
        case CelestiaGamepadAction::RollRight:
            up ? appCore.KeyUp(static_cast<int32_t>(VirtualKey::NumberPad9), 0) : appCore.KeyDown(static_cast<int32_t>(VirtualKey::NumberPad9), 0);
            break;
        default:
            break;
        }
    }
}
