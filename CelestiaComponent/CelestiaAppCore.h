#pragma once

#include "CelestiaDestination.h"
#include "CelestiaSimulation.h"
#include "CelestiaScript.h"
#include "CelestiaAppCore.g.h"
#include <celestia/celestiacore.h>

namespace winrt::CelestiaComponent::implementation
{
    struct CelestiaAppCore : CelestiaAppCoreT<CelestiaAppCore>
    {
        CelestiaAppCore();

        bool StartSimulation(hstring const& configFileName, array_view<hstring const> extraDirectories, CelestiaComponent::CelestiaLoadCallback const &callback);
        bool StartRenderer();
        void Tick();
        void Draw();
        void Resize(int32_t width, int32_t height);
        void Start();
        void SetDPI(int32_t dpi);
        void MouseMove(float x, float y, int32_t button);
        void MouseButtonUp(float x, float y, int32_t button);
        void MouseButtonDown(float x, float y, int32_t button);
        void CharEnter(int16_t input);
        void SetContextMenuHandler(CelestiaComponent::CelestiaContextMenuCallback const& handler);
        void SetFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize);
        void SetTitleFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize);
        void SetRenderFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize, CelestiaComponent::CelestiaFontStyle fontStyle);
        void RunScript(hstring const& path);
        void GoToURL(hstring const& url);
        com_array<CelestiaComponent::CelestiaDestination> Destinations();
        CelestiaComponent::CelestiaSimulation Simulation();
        hstring CurrentURL();
        hstring RenderInfo();
        int64_t Pointer();

        static void InitGL();
        static void SetLocaleDirectory(hstring const& localeDirectory, hstring const& locale);
        static hstring LocalizedString(hstring const& original);
        static com_array<CelestiaComponent::CelestiaScript> ReadScripts(hstring const& directory, bool deepScan);

        bool ShowStars();
        void ShowStars(bool showStars);
        bool ShowPlanets();
        void ShowPlanets(bool showPlanets);
        bool ShowDwarfPlanets();
        void ShowDwarfPlanets(bool showDwarfPlanets);
        bool ShowMoons();
        void ShowMoons(bool showMoons);
        bool ShowMinorMoons();
        void ShowMinorMoons(bool showMinorMoons);
        bool ShowAsteroids();
        void ShowAsteroids(bool showAsteroids);
        bool ShowComets();
        void ShowComets(bool showComets);
        bool ShowSpacecrafts();
        void ShowSpacecrafts(bool showSpacecrafts);
        bool ShowGalaxies();
        void ShowGalaxies(bool showGalaxies);
        bool ShowGlobulars();
        void ShowGlobulars(bool showGlobulars);
        bool ShowNebulae();
        void ShowNebulae(bool showNebulae);
        bool ShowOpenClusters();
        void ShowOpenClusters(bool showOpenClusters);
        bool ShowDiagrams();
        void ShowDiagrams(bool showDiagrams);
        bool ShowBoundaries();
        void ShowBoundaries(bool showBoundaries);
        bool ShowCloudMaps();
        void ShowCloudMaps(bool showCloudMaps);
        bool ShowNightMaps();
        void ShowNightMaps(bool showNightMaps);
        bool ShowAtmospheres();
        void ShowAtmospheres(bool showAtmospheres);
        bool ShowCometTails();
        void ShowCometTails(bool showCometTails);
        bool ShowPlanetRings();
        void ShowPlanetRings(bool showPlanetRings);
        bool ShowMarkers();
        void ShowMarkers(bool showMarkers);
        bool ShowOrbits();
        void ShowOrbits(bool showOrbits);
        bool ShowPartialTrajectories();
        void ShowPartialTrajectories(bool showPartialTrajectories);
        bool ShowSmoothLines();
        void ShowSmoothLines(bool showSmoothLines);
        bool ShowEclipseShadows();
        void ShowEclipseShadows(bool showEclipseShadows);
        bool ShowRingShadows();
        void ShowRingShadows(bool showRingShadows);
        bool ShowCloudShadows();
        void ShowCloudShadows(bool showCloudShadows);
        bool ShowAutoMag();
        void ShowAutoMag(bool showAutoMag);
        bool ShowCelestialSphere();
        void ShowCelestialSphere(bool showCelestialSphere);
        bool ShowEclipticGrid();
        void ShowEclipticGrid(bool showEclipticGrid);
        bool ShowHorizonGrid();
        void ShowHorizonGrid(bool showHorizonGrid);
        bool ShowGalacticGrid();
        void ShowGalacticGrid(bool showGalacticGrid);
        bool ShowStarLabels();
        void ShowStarLabels(bool showStarLabels);
        bool ShowPlanetLabels();
        void ShowPlanetLabels(bool showPlanetLabels);
        bool ShowMoonLabels();
        void ShowMoonLabels(bool showMoonLabels);
        bool ShowConstellationLabels();
        void ShowConstellationLabels(bool showConstellationLabels);
        bool ShowGalaxyLabels();
        void ShowGalaxyLabels(bool showGalaxyLabels);
        bool ShowGlobularLabels();
        void ShowGlobularLabels(bool showGlobularLabels);
        bool ShowNebulaLabels();
        void ShowNebulaLabels(bool showNebulaLabels);
        bool ShowOpenClusterLabels();
        void ShowOpenClusterLabels(bool showOpenClusterLabels);
        bool ShowAsteroidLabels();
        void ShowAsteroidLabels(bool showAsteroidLabels);
        bool ShowSpacecraftLabels();
        void ShowSpacecraftLabels(bool showSpacecraftLabels);
        bool ShowLocationLabels();
        void ShowLocationLabels(bool showLocationLabels);
        bool ShowCometLabels();
        void ShowCometLabels(bool showCometLabels);
        bool ShowDwarfPlanetLabels();
        void ShowDwarfPlanetLabels(bool showDwarfPlanetLabels);
        bool ShowMinorMoonLabels();
        void ShowMinorMoonLabels(bool showMinorMoonLabels);
        bool ShowLatinConstellationLabels();
        void ShowLatinConstellationLabels(bool showLatinConstellationLabels);
        bool ShowPlanetOrbits();
        void ShowPlanetOrbits(bool showPlanetOrbits);
        bool ShowMoonOrbits();
        void ShowMoonOrbits(bool showMoonOrbits);
        bool ShowAsteroidOrbits();
        void ShowAsteroidOrbits(bool showAsteroidOrbits);
        bool ShowSpacecraftOrbits();
        void ShowSpacecraftOrbits(bool showSpacecraftOrbits);
        bool ShowCometOrbits();
        void ShowCometOrbits(bool showCometOrbits);
        bool ShowStellarOrbits();
        void ShowStellarOrbits(bool showStellarOrbits);
        bool ShowDwarfPlanetOrbits();
        void ShowDwarfPlanetOrbits(bool showDwarfPlanetOrbits);
        bool ShowMinorMoonOrbits();
        void ShowMinorMoonOrbits(bool showMinorMoonOrbits);
        bool ShowCityLabels();
        void ShowCityLabels(bool showCityLabels);
        bool ShowObservatoryLabels();
        void ShowObservatoryLabels(bool showObservatoryLabels);
        bool ShowLandingSiteLabels();
        void ShowLandingSiteLabels(bool showLandingSiteLabels);
        bool ShowCraterLabels();
        void ShowCraterLabels(bool showCraterLabels);
        bool ShowVallisLabels();
        void ShowVallisLabels(bool showVallisLabels);
        bool ShowMonsLabels();
        void ShowMonsLabels(bool showMonsLabels);
        bool ShowPlanumLabels();
        void ShowPlanumLabels(bool showPlanumLabels);
        bool ShowChasmaLabels();
        void ShowChasmaLabels(bool showChasmaLabels);
        bool ShowPateraLabels();
        void ShowPateraLabels(bool showPateraLabels);
        bool ShowMareLabels();
        void ShowMareLabels(bool showMareLabels);
        bool ShowRupesLabels();
        void ShowRupesLabels(bool showRupesLabels);
        bool ShowTesseraLabels();
        void ShowTesseraLabels(bool showTesseraLabels);
        bool ShowRegioLabels();
        void ShowRegioLabels(bool showRegioLabels);
        bool ShowChaosLabels();
        void ShowChaosLabels(bool showChaosLabels);
        bool ShowTerraLabels();
        void ShowTerraLabels(bool showTerraLabels);
        bool ShowAstrumLabels();
        void ShowAstrumLabels(bool showAstrumLabels);
        bool ShowCoronaLabels();
        void ShowCoronaLabels(bool showCoronaLabels);
        bool ShowDorsumLabels();
        void ShowDorsumLabels(bool showDorsumLabels);
        bool ShowFossaLabels();
        void ShowFossaLabels(bool showFossaLabels);
        bool ShowCatenaLabels();
        void ShowCatenaLabels(bool showCatenaLabels);
        bool ShowMensaLabels();
        void ShowMensaLabels(bool showMensaLabels);
        bool ShowRimaLabels();
        void ShowRimaLabels(bool showRimaLabels);
        bool ShowUndaeLabels();
        void ShowUndaeLabels(bool showUndaeLabels);
        bool ShowReticulumLabels();
        void ShowReticulumLabels(bool showReticulumLabels);
        bool ShowPlanitiaLabels();
        void ShowPlanitiaLabels(bool showPlanitiaLabels);
        bool ShowLineaLabels();
        void ShowLineaLabels(bool showLineaLabels);
        bool ShowFluctusLabels();
        void ShowFluctusLabels(bool showFluctusLabels);
        bool ShowFarrumLabels();
        void ShowFarrumLabels(bool showFarrumLabels);
        bool ShowEruptiveCenterLabels();
        void ShowEruptiveCenterLabels(bool showEruptiveCenterLabels);
        bool ShowOtherLabels();
        void ShowOtherLabels(bool showOtherLabels);
        bool ShowI18nConstellationLabels();
        void ShowI18nConstellationLabels(bool showI18nConstellationLabels);
        int32_t Resolution();
        void Resolution(int32_t resolution);
        int32_t StarStyle();
        void StarStyle(int32_t starStyle);
        int32_t HudDetail();
        void HudDetail(int32_t hudDetail);
        int32_t DateFormat();
        void DateFormat(int32_t dateFormat);
        double AmbientLightLevel();
        void AmbientLightLevel(double ambientLightLevel);
        double FaintestVisible();
        void FaintestVisible(double faintestVisible);
        double GalaxyBrightness();
        void GalaxyBrightness(double galaxyBrightness);
        double MinimumFeatureSize();
        void MinimumFeatureSize(double minimumFeatureSize);

    private:
        CelestiaCore* core;
        CelestiaComponent::CelestiaSimulation sim;
    };
}

namespace winrt::CelestiaComponent::factory_implementation
{
    struct CelestiaAppCore : CelestiaAppCoreT<CelestiaAppCore, implementation::CelestiaAppCore>
    {
    };
}
