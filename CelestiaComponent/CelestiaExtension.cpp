#include "pch.h"
#include "CelestiaExtension.h"
#if __has_include("CelestiaExtension.g.cpp")
#include "CelestiaExtension.g.cpp"
#endif

#include <fmt/printf.h>
#include <fmt/xchar.h>

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
        if (name == L"ShowStars")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowStars;
        else if (name == L"ShowPlanets")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanets;
        else if (name == L"ShowDwarfPlanets")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanets;
        else if (name == L"ShowMoons")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoons;
        else if (name == L"ShowMinorMoons")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoons;
        else if (name == L"ShowAsteroids")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroids;
        else if (name == L"ShowComets")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowComets;
        else if (name == L"ShowSpacecrafts")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecrafts;
        else if (name == L"ShowGalaxies")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxies;
        else if (name == L"ShowGlobulars")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobulars;
        else if (name == L"ShowNebulae")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulae;
        else if (name == L"ShowOpenClusters")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusters;
        else if (name == L"ShowDiagrams")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowDiagrams;
        else if (name == L"ShowBoundaries")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowBoundaries;
        else if (name == L"ShowCloudMaps")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudMaps;
        else if (name == L"ShowNightMaps")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowNightMaps;
        else if (name == L"ShowAtmospheres")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowAtmospheres;
        else if (name == L"ShowCometTails")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometTails;
        else if (name == L"ShowPlanetRings")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetRings;
        else if (name == L"ShowMarkers")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowMarkers;
        else if (name == L"ShowOrbits")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowOrbits;
        else if (name == L"ShowFadingOrbits")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowFadingOrbits;
        else if (name == L"ShowEcliptic")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowEcliptic;
        else if (name == L"ShowPartialTrajectories")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowPartialTrajectories;
        else if (name == L"ShowSmoothLines")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowSmoothLines;
        else if (name == L"ShowEclipseShadows")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipseShadows;
        else if (name == L"ShowRingShadows")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowRingShadows;
        else if (name == L"ShowCloudShadows")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudShadows;
        else if (name == L"ShowAutoMag")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowAutoMag;
        else if (name == L"ShowCelestialSphere")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowCelestialSphere;
        else if (name == L"ShowEclipticGrid")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipticGrid;
        else if (name == L"ShowHorizonGrid")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowHorizonGrid;
        else if (name == L"ShowGalacticGrid")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalacticGrid;
        else if (name == L"ShowStarLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowStarLabels;
        else if (name == L"ShowPlanetLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetLabels;
        else if (name == L"ShowMoonLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonLabels;
        else if (name == L"ShowConstellationLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowConstellationLabels;
        else if (name == L"ShowGalaxyLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxyLabels;
        else if (name == L"ShowGlobularLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobularLabels;
        else if (name == L"ShowNebulaLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulaLabels;
        else if (name == L"ShowOpenClusterLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusterLabels;
        else if (name == L"ShowAsteroidLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidLabels;
        else if (name == L"ShowSpacecraftLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftLabels;
        else if (name == L"ShowLocationLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowLocationLabels;
        else if (name == L"ShowCometLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometLabels;
        else if (name == L"ShowDwarfPlanetLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetLabels;
        else if (name == L"ShowMinorMoonLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonLabels;
        else if (name == L"ShowLatinConstellationLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowLatinConstellationLabels;
        else if (name == L"ShowPlanetOrbits")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetOrbits;
        else if (name == L"ShowMoonOrbits")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonOrbits;
        else if (name == L"ShowAsteroidOrbits")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidOrbits;
        else if (name == L"ShowSpacecraftOrbits")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftOrbits;
        else if (name == L"ShowCometOrbits")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometOrbits;
        else if (name == L"ShowStellarOrbits")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowStellarOrbits;
        else if (name == L"ShowDwarfPlanetOrbits")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetOrbits;
        else if (name == L"ShowMinorMoonOrbits")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonOrbits;
        else if (name == L"ShowCityLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowCityLabels;
        else if (name == L"ShowObservatoryLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowObservatoryLabels;
        else if (name == L"ShowLandingSiteLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowLandingSiteLabels;
        else if (name == L"ShowCraterLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowCraterLabels;
        else if (name == L"ShowVallisLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowVallisLabels;
        else if (name == L"ShowMonsLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowMonsLabels;
        else if (name == L"ShowPlanumLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanumLabels;
        else if (name == L"ShowChasmaLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowChasmaLabels;
        else if (name == L"ShowPateraLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowPateraLabels;
        else if (name == L"ShowMareLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowMareLabels;
        else if (name == L"ShowRupesLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowRupesLabels;
        else if (name == L"ShowTesseraLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowTesseraLabels;
        else if (name == L"ShowRegioLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowRegioLabels;
        else if (name == L"ShowChaosLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowChaosLabels;
        else if (name == L"ShowTerraLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowTerraLabels;
        else if (name == L"ShowAstrumLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowAstrumLabels;
        else if (name == L"ShowCoronaLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowCoronaLabels;
        else if (name == L"ShowDorsumLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowDorsumLabels;
        else if (name == L"ShowFossaLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowFossaLabels;
        else if (name == L"ShowCatenaLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowCatenaLabels;
        else if (name == L"ShowMensaLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowMensaLabels;
        else if (name == L"ShowRimaLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowRimaLabels;
        else if (name == L"ShowUndaeLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowUndaeLabels;
        else if (name == L"ShowReticulumLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowReticulumLabels;
        else if (name == L"ShowPlanitiaLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanitiaLabels;
        else if (name == L"ShowLineaLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowLineaLabels;
        else if (name == L"ShowFluctusLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowFluctusLabels;
        else if (name == L"ShowFarrumLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowFarrumLabels;
        else if (name == L"ShowEruptiveCenterLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowEruptiveCenterLabels;
        else if (name == L"ShowOtherLabels")
            return CelestiaComponent::CelestiaSettingBooleanEntry::ShowOtherLabels;
        return CelestiaComponent::CelestiaSettingBooleanEntry::None;
    }

    CelestiaComponent::CelestiaSettingInt32Entry CelestiaExtension::GetInt32EntryByName(hstring const& name)
    {
        if (name == L"Resolution")
            return CelestiaComponent::CelestiaSettingInt32Entry::Resolution;
        else if (name == L"StarStyle")
            return CelestiaComponent::CelestiaSettingInt32Entry::StarStyle;
        else if (name == L"HudDetail")
            return CelestiaComponent::CelestiaSettingInt32Entry::HudDetail;
        else if (name == L"DateFormat")
            return CelestiaComponent::CelestiaSettingInt32Entry::DateFormat;
        else if (name == L"TimeZone")
            return CelestiaComponent::CelestiaSettingInt32Entry::TimeZone;
        else if (name == L"MeasurementSystem")
            return CelestiaComponent::CelestiaSettingInt32Entry::MeasurementSystem;
        else if (name == L"TemperatureScale")
            return CelestiaComponent::CelestiaSettingInt32Entry::TemperatureScale;
        else if (name == L"ScriptSystemAccessPolicy")
            return CelestiaComponent::CelestiaSettingInt32Entry::ScriptSystemAccessPolicy;
        return CelestiaComponent::CelestiaSettingInt32Entry::None;
    }

    CelestiaComponent::CelestiaSettingSingleEntry CelestiaExtension::GetSingleEntryByName(hstring const& name)
    {
        if (name == L"AmbientLightLevel")
            return CelestiaComponent::CelestiaSettingSingleEntry::AmbientLightLevel;
        else if (name == L"FaintestVisible")
            return CelestiaComponent::CelestiaSettingSingleEntry::FaintestVisible;
        else if (name == L"GalaxyBrightness")
            return CelestiaComponent::CelestiaSettingSingleEntry::GalaxyBrightness;
        else if (name == L"MinimumFeatureSize")
            return CelestiaComponent::CelestiaSettingSingleEntry::MinimumFeatureSize;
        else if (name == L"DistanceLimit")
            return CelestiaComponent::CelestiaSettingSingleEntry::DistanceLimit;
        return CelestiaComponent::CelestiaSettingSingleEntry::None;
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
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOtherLabels:
            return L"ShowOtherLabels";
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
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOtherLabels:
            appCore.ShowOtherLabels(value);
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
        case CelestiaComponent::CelestiaSettingBooleanEntry::ShowOtherLabels:
            return appCore.ShowOtherLabels();
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
        default:
            break;
        }
        return 0.0f;
    }


    hstring JoinLines(std::vector<std::wstring> const& lines)
    {
        hstring result;
        for (auto const& line : lines)
        {
            if (!result.empty())
                result = result + L"\n";
            result = result + hstring(line);
        }
        return result;
    }

    hstring GetBodyOverview(CelestiaBody const& body, CelestiaComponent::LocalizationProvider const& localizationProvider, CelestiaAppCore const& appCore)
    {
        std::vector<std::wstring> lines;
        auto radius = body.Radius();
        std::wstring radiusString = L"";
        const float oneMiInKm = 1.609344f;
        const float oneFtInKm = 0.0003048f;
        if (appCore.MeasurementSystem() == (int32_t)CelestiaMeasurementSystem::Imperial)
        {
            if (radius > oneMiInKm)
                radiusString = fmt::sprintf(std::wstring(localizationProvider(L"%d mi")), (int)(radius / oneMiInKm));
            else
                radiusString = fmt::sprintf(std::wstring(localizationProvider(L"%d ft")), (int)(radius / oneFtInKm));
        }
        else
        {
            if (radius > 1.0f)
                radiusString = fmt::sprintf(std::wstring(localizationProvider(L"%d km")), (int)radius);
            else
                radiusString = fmt::sprintf(std::wstring(localizationProvider(L"%d m")), (int)(radius * 1000.0f));
        }
        if (body.IsEllipsoid())
            lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"Equatorial radius: %s")), radiusString));
        else
            lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"Size: %s")), radiusString));
        auto time = appCore.Simulation().Time();
        auto orbit = body.OrbitAtTime(time);
        auto rotation = body.RotationModelAtTime(time);
        auto orbitalPeriod = orbit.IsPeriodic() ? orbit.Period() : 0.0;
        if (rotation.IsPeriodic() && body.Type() != CelestiaBodyType::spacecraft)
        {
            auto rotPeriod = rotation.Period();
            auto dayLength = 0.0;
            if (orbit.IsPeriodic())
            {
                auto siderealDaysPerYear = orbitalPeriod / rotPeriod;
                auto solarDaysPerYear = siderealDaysPerYear - 1.0;
                if (solarDaysPerYear > 0.0001)
                {
                    dayLength = orbitalPeriod / (siderealDaysPerYear - 1.0);
                }
            }
            hstring unitTemplate = L"";
            if (rotPeriod < 2.0)
            {
                rotPeriod *= 24;
                dayLength *= 24;

                unitTemplate = localizationProvider(L"%.2f hours");
            }
            else
            {
                unitTemplate = localizationProvider(L"%.2f days");
            }
            lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"Sidereal rotation period: %s")), fmt::sprintf(std::wstring(unitTemplate), rotPeriod)));

            if (dayLength != 0.0)
            {
                lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"Length of day: %s")), fmt::sprintf(std::wstring(unitTemplate), dayLength)));
            }
        }
        if (body.HasRings())
        {
            lines.push_back(std::wstring(localizationProvider(L"Has rings")));
        }

        if (body.HasAtmosphere())
        {
            lines.push_back(std::wstring(localizationProvider(L"Has atmosphere")));
        }

        auto timeline{ body.Timeline() };
        if (timeline.PhaseCount() > 0)
        {
            using namespace Windows::Globalization::DateTimeFormatting;
            DateTimeFormatter dateFormatter{ L"shortdate shorttime" };

            auto startTime = timeline.PhaseAtIndex(0).StartTime();
            auto endTime = timeline.PhaseAtIndex(0).EndTime();
            if (startTime)
            {
                auto time = startTime.Value();
                lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"Start time: %s")), std::wstring(dateFormatter.Format(time))));
            }
            if (endTime)
            {
                auto time = endTime.Value();
                lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"End time: %s")), std::wstring(dateFormatter.Format(time))));
            }
        }

        return JoinLines(lines);
    }

    hstring GetStarOverview(CelestiaStar const& star, CelestiaComponent::LocalizationProvider const& localizationProvider, CelestiaAppCore const& appCore)
    {
        std::vector<std::wstring> lines;

        lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"Spectral type: %s")), std::wstring(star.SpectralType())));

        auto time = appCore.Simulation().Time();
        auto celPos = star.PositionAtTime(time).OffsetFrom(CelestiaUniversalCoord::Zero());
        auto eqPos = CelestiaHelper::EclipticToEquatorial(CelestiaHelper::CelToJ2000Ecliptic(celPos));
        auto sph = CelestiaHelper::RectToSpherical(eqPos);

        CelestiaDMS hms{ sph.X() };
        lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"RA: %dh %dm %.2fs")), hms.Hours(), hms.Minutes(), hms.Seconds()));

        CelestiaDMS dms{ sph.Y() };
        lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"DEC: %d\u00b0 %d\u2032 %.2f\u2033")), dms.Hours(), dms.Minutes(), dms.Seconds()));

        return JoinLines(lines);
    }

    hstring GetDSOOveriew(CelestiaDSO const& dso, CelestiaComponent::LocalizationProvider const& localizationProvider, CelestiaAppCore const&)
    {
        std::vector<std::wstring> lines;

        if (!dso.Description().empty())
            lines.push_back(std::wstring(dso.Description()));

        auto celPos = dso.Position();
        auto eqPos = CelestiaHelper::EclipticToEquatorial(CelestiaHelper::CelToJ2000Ecliptic(celPos));
        auto sph = CelestiaHelper::RectToSpherical(eqPos);

        CelestiaDMS hms{ sph.X() };
        lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"RA: %dh %dm %.2fs")), hms.Hours(), hms.Minutes(), hms.Seconds()));

        CelestiaDMS dms{ sph.Y() };
        lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"DEC: %d\u00b0 %d\u2032 %.2f\u2033")), dms.Hours(), dms.Minutes(), dms.Seconds()));

        auto galPos = CelestiaHelper::EquatorialToGalactic(eqPos);
        sph = CelestiaHelper::RectToSpherical(galPos);

        dms = CelestiaDMS(sph.X());
        lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"L: %d\u00b0 %d\u2032 %.2f\u2033")), dms.Hours(), dms.Minutes(), dms.Seconds()));

        dms = CelestiaDMS(sph.Y());
        lines.push_back(fmt::sprintf(std::wstring(localizationProvider(L"B: %d\u00b0 %d\u2032 %.2f\u2033")), dms.Hours(), dms.Minutes(), dms.Seconds()));

        return JoinLines(lines);
    }

    hstring CelestiaExtension::GetOverviewForSelection(CelestiaComponent::CelestiaSelection const& selection, CelestiaComponent::LocalizationProvider const& localizationProvider, CelestiaComponent::CelestiaAppCore const& appCore)
    {
        auto obj = selection.Object();
        if (obj == nullptr) return L"";
        auto body = obj.try_as<CelestiaComponent::CelestiaBody>();
        if (body != nullptr)
        {
            return GetBodyOverview(body, localizationProvider, appCore);
        }
        auto star = obj.try_as<CelestiaComponent::CelestiaStar>();
        if (star != nullptr)
        {
            return GetStarOverview(star, localizationProvider, appCore);
        }
        auto dso = obj.try_as<CelestiaComponent::CelestiaDSO>();
        if (dso != nullptr)
        {
            return GetDSOOveriew(dso, localizationProvider, appCore);
        }
        return localizationProvider(L"No overview available.");
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
                appCore.CharEnter(115);
            break;
        case CelestiaGamepadAction::ReverseSpeed:
            if (up)
                appCore.CharEnter(113);
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
                appCore.CharEnter(103);
            break;
        case CelestiaGamepadAction::Esc:
            if (up)
                appCore.CharEnter(27);
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
