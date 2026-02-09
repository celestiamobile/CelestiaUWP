// CelestiaAppCore.h
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#pragma once

#include "CelestiaDestination.h"
#include "CelestiaSimulation.h"
#include "CelestiaScript.h"
#include "ShowContextMenuArgs.g.h"
#include "ChangeCursorArgs.g.h"
#include "FatalErrorArgs.g.h"
#include "SystemAccessRequestArgs.g.h"
#include "CelestiaAppCore.g.h"
#include <celestia/celestiacore.h>
#include <winrt/Windows.Globalization.DateTimeFormatting.h>

namespace winrt::CelestiaComponent::implementation
{
    struct ShowContextMenuArgs : ShowContextMenuArgsT<ShowContextMenuArgs>
    {
        ShowContextMenuArgs(float x, float y, CelestiaComponent::CelestiaSelection const& selection) : x(x), y(y), selection(selection) {}
        float X() { return x; }
        float Y() { return y; }
        CelestiaComponent::CelestiaSelection Selection() { return selection; }

    private:
        float x;
        float y;
        CelestiaComponent::CelestiaSelection selection;
    };

    struct ChangeCursorArgs : ChangeCursorArgsT<ChangeCursorArgs>
    {
        ChangeCursorArgs(CelestiaComponent::Cursor cursor) : cursor(cursor) {}
        CelestiaComponent::Cursor Cursor() { return cursor; }
    private:
        CelestiaComponent::Cursor cursor;
    };

    struct FatalErrorArgs : FatalErrorArgsT<FatalErrorArgs>
    {
        FatalErrorArgs(hstring const& message) : message(message) {}
        hstring Message() { return message; }
    private:
        hstring message;
    };

    struct SystemAccessRequestArgs : SystemAccessRequestArgsT<SystemAccessRequestArgs>
    {
        SystemAccessRequestArgs(CelestiaComponent::SystemAccessRequestResult result) : result(result) {}
        CelestiaComponent::SystemAccessRequestResult Result() { return result; }
        void Result(CelestiaComponent::SystemAccessRequestResult newValue) { result = newValue; }
    private:
        CelestiaComponent::SystemAccessRequestResult result;
    };

    struct CelestiaAppCore : CelestiaAppCoreT<CelestiaAppCore>
    {
        CelestiaAppCore();

        bool StartSimulation(hstring const& configFileName, array_view<hstring const> extraDirectories, CelestiaComponent::CelestiaLoadCallback const &callback);
        bool StartRenderer();
        void Tick();
        void Draw();
        void Resize(int32_t width, int32_t height);
        int32_t GetWidth();
        int32_t GetHeight();
        void Start();
        int32_t ScreenDPI();
        void ScreenDPI(int32_t dpi);
        float TextScaleFactor();
        void TextScaleFactor(float textScaleFactor);
        void SetSafeAreaInsets(int32_t left, int32_t top, int32_t right, int32_t bottom);
        void MouseMove(float x, float y, CelestiaComponent::CelestiaMouseButton button);
        void MouseButtonUp(float x, float y, CelestiaComponent::CelestiaMouseButton button);
        void MouseButtonDown(float x, float y, CelestiaComponent::CelestiaMouseButton button);
        void MouseWheel(float motion, int32_t modifiers);
        void CharEnter(int16_t input);
        void CharEnter(int16_t input, int32_t modifiers);
        void KeyUp(int32_t key, int32_t modifiers);
        void KeyDown(int32_t key, int32_t modifiers);
        void JoystickButtonDown(CelestiaComponent::CelestiaJoystickButton button);
        void JoystickButtonUp(CelestiaComponent::CelestiaJoystickButton button);
        void JoystickAxis(CelestiaComponent::CelestiaJoystickAxis axis, float amount);
        void SetHudFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize);
        void SetHudTitleFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize);
        void SetRenderFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize, CelestiaComponent::CelestiaFontStyle fontStyle);
        void RunScript(hstring const& path);
        void RunDemo();
        void GoToURL(hstring const& url);
        bool SaveScreenshot(hstring const& filePath);
        com_array<CelestiaComponent::CelestiaDestination> Destinations();
        CelestiaComponent::CelestiaSimulation Simulation();
        hstring CurrentURL();
        hstring RenderInfo();
        int64_t Pointer();

        static void InitGL();
        static hstring Language();
        static void SetLocaleDirectory(hstring const& localeDirectory, hstring const& locale);
        static void SetUpLocale();
        static hstring LocalizedString(hstring const& original, hstring const& domain);
        static hstring LocalizedString(hstring const& original, hstring const& context, hstring const& domain);
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
        bool ShowFadingOrbits();
        void ShowFadingOrbits(bool showFadingOrbits);
        bool ShowEcliptic();
        void ShowEcliptic(bool showEcliptic);
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
        bool ShowCollumLabels();
        void ShowCollumLabels(bool showCollumLabels);
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
        int32_t StarColors();
        void StarColors(int32_t starColors);
        float AmbientLightLevel();
        void AmbientLightLevel(float ambientLightLevel);
        float FaintestVisible();
        void FaintestVisible(float faintestVisible);
        float GalaxyBrightness();
        void GalaxyBrightness(float galaxyBrightness);
        float MinimumFeatureSize();
        void MinimumFeatureSize(float minimumFeatureSize);
        float DistanceLimit();
        void DistanceLimit(float distanceLimit);
        float TintSaturation();
        void TintSaturation(float tintSaturation);

        bool ShowBodyAxes();
        void ShowBodyAxes(bool showBodyAxes);
        bool ShowFrameAxes();
        void ShowFrameAxes(bool showFrameAxes);
        bool ShowSunDirection();
        void ShowSunDirection(bool showSunDirection);
        bool ShowVelocityVector();
        void ShowVelocityVector(bool showVelocityVector);
        bool ShowPlanetographicGrid();
        void ShowPlanetographicGrid(bool showPlanetographicGrid);
        bool ShowTerminator();
        void ShowTerminator(bool showTerminator);

        bool EnableReverseWheel();
        void EnableReverseWheel(bool value);
        bool EnableRayBasedDragging();
        void EnableRayBasedDragging(bool value);
        bool EnableFocusZooming();
        void EnableFocusZooming(bool value);
        bool EnableAlignCameraToSurfaceOnLand();
        void EnableAlignCameraToSurfaceOnLand(bool value);

        int32_t TimeZone();
        void TimeZone(int32_t timeZone);

        int32_t MeasurementSystem();
        void MeasurementSystem(int32_t measurementSystem);
        int32_t TemperatureScale();
        void TemperatureScale(int32_t temperatureScale);
        int32_t ScriptSystemAccessPolicy();
        void ScriptSystemAccessPolicy(int32_t scriptSystemAccessPolicy);

        CelestiaComponent::CelestiaLayoutDirection LayoutDirection();
        void LayoutDirection(CelestiaComponent::CelestiaLayoutDirection layoutDirection);

        float PickTolerance();
        void PickTolerance(float pickTolerance);

        event_token ShowContextMenu(Windows::Foundation::EventHandler<CelestiaComponent::ShowContextMenuArgs> const&);
        void ShowContextMenu(event_token const&);

        event_token ChangeCursor(Windows::Foundation::EventHandler<CelestiaComponent::ChangeCursorArgs> const&);
        void ChangeCursor(event_token const&);

        event_token FatalError(Windows::Foundation::EventHandler<CelestiaComponent::FatalErrorArgs> const&);
        void FatalError(event_token const&);

        event_token SystemAccessRequest(Windows::Foundation::EventHandler<CelestiaComponent::SystemAccessRequestArgs> const&);
        void SystemAccessRequest(event_token const&);

        event<Windows::Foundation::EventHandler<CelestiaComponent::ShowContextMenuArgs>> showContextMenuEvent;
        event<Windows::Foundation::EventHandler<CelestiaComponent::ChangeCursorArgs>> changeCursorEvent;
        event<Windows::Foundation::EventHandler<CelestiaComponent::FatalErrorArgs>> fatalErrorEvent;
        event<Windows::Foundation::EventHandler<CelestiaComponent::SystemAccessRequestArgs>> systemAccessRequestEvent;

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
