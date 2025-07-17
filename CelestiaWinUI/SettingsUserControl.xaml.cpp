#include "pch.h"
#include "SettingsUserControl.xaml.h"
#if __has_include("SettingsUserControl.g.cpp")
#include "SettingsUserControl.g.cpp"
#endif
#if __has_include("SettingsNavigationItemGroup.g.cpp")
#include "SettingsNavigationItemGroup.g.cpp"
#endif

using namespace winrt;
using namespace CelestiaAppComponent;
using namespace CelestiaComponent;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;

namespace winrt::CelestiaWinUI::implementation
{
    SettingsNavigationItemGroup::SettingsNavigationItemGroup(hstring const& title, Collections::IObservableVector<IInspectable> const& items, bool showRestartHint) : title(title), items(items), showRestartHint(showRestartHint) {};

    hstring SettingsNavigationItemGroup::Title()
    {
        return title;
    }

    Collections::IObservableVector<IInspectable> SettingsNavigationItemGroup::Items()
    {
        return items;
    }

    bool SettingsNavigationItemGroup::ShowRestartHint()
    {
        return showRestartHint;
    }

    SettingsUserControl::SettingsUserControl(CelestiaAppCore const& appCore, CelestiaRenderer const& renderer, AppSettings const& appSettings, Windows::Storage::ApplicationDataContainer const& localSettings, Collections::IVector<hstring> const& availableLanguages)
    {
        itemGroups = single_threaded_observable_vector<CelestiaWinUI::SettingsNavigationItemGroup>();
        std::vector<IInspectable> displaySettingItems =
        {
            SettingHeaderItem(LocalizationHelper::Localize(L"Objects", L"")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars", L"Tab for stars in Star Browser"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStars, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars", L"Tab for stars in Star Browser"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStars, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Planets", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanets, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Dwarf Planets", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanets, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Moons", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoons, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Minor Moons", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoons, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Asteroids", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroids, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Comets", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowComets, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Spacecraft", L"Plural"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecrafts, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Galaxies", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxies, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Nebulae", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulae, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Globulars", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobulars, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Open Clusters", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusters, localSettings),
            SettingHeaderItem(LocalizationHelper::Localize(L"Features", L"")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Clouds", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudMaps, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Atmospheres", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAtmospheres, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Comet Tails", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometTails, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Planet Rings", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetRings, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Ring Shadows", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowRingShadows, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Eclipse Shadows", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipseShadows, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Cloud Shadows", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudShadows, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Night Lights", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowNightMaps, localSettings),
        };
        auto displaySettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        displaySettingItemGroupItems.ReplaceAll(displaySettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Display", L"Display settings"), displaySettingItemGroupItems, false));

        std::vector<IInspectable> guidesSettingItems =
        {
             SettingHeaderItem(LocalizationHelper::Localize(L"Orbits", L"")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Orbits", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Fading Orbits", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowFadingOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Partial Trajectories", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPartialTrajectories, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars", L"Tab for stars in Star Browser"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStellarOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Planets", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Dwarf Planets", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Moons", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Minor Moons", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Asteroids", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Comets", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Spacecraft", L"Plural"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftOrbits, localSettings),

             SettingHeaderItem(LocalizationHelper::Localize(L"Constellations", L"")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Diagrams", L"Show constellation diagrams"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDiagrams, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Labels", L"Constellation labels"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowConstellationLabels, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Labels in Latin", L"Constellation labels in Latin"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowLatinConstellationLabels, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Boundaries", L"Show constellation boundaries"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowBoundaries, localSettings),

             SettingHeaderItem(LocalizationHelper::Localize(L"Grids", L"")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Equatorial", L"Grids"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCelestialSphere, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Ecliptic", L"Grids"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipticGrid, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Horizontal", L"Grids"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowHorizonGrid, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Galactic", L"Grids"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalacticGrid, localSettings),

             SettingHeaderItem(LocalizationHelper::Localize(L"Miscellaneous", L"")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Markers", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMarkers, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Ecliptic Line", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEcliptic, localSettings),
        };
        auto guidesSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        guidesSettingItemGroupItems.ReplaceAll(guidesSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Guides", L"Grids, labels, orbits, markers"), guidesSettingItemGroupItems, false));

        std::vector<IInspectable> labelsSettingItems =
        {
            SettingHeaderItem(LocalizationHelper::Localize(L"Objects", L"")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars", L"Tab for stars in Star Browser"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStarLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Planets", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Dwarf Planets", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Moons", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Minor Moons", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Asteroids", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Comets", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Spacecraft", L"Plural"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Galaxies", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxyLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Nebulae", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulaLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Globulars", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobularLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Open Clusters", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusterLabels, localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Locations", L"Location labels to display")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Locations", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowLocationLabels, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Minimum Labeled Feature Size", L"Minimum feature size that we should display a label for"), appCore, renderer, CelestiaComponent::CelestiaSettingSingleEntry::MinimumFeatureSize, 0.0f, 99.0f, 1.0f, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Cities", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCityLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Observatories", L"Location labels"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowObservatoryLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Landing Sites", L"Location labels"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowLandingSiteLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Montes (Mountains)", L"Location labels"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMonsLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Maria (Seas)", L"Location labels"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMareLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Craters", L"Location labels"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCraterLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Valles (Valleys)", L"Location labels"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowVallisLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Terrae (Land masses)", L"Location labels"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowTerraLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Volcanoes", L"Location labels"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEruptiveCenterLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Other", L"Other location labels; Android/iOS, Other objects to choose from in Eclipse Finder"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOtherLabels, localSettings),
        };
        auto labelsSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        labelsSettingItemGroupItems.ReplaceAll(labelsSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Labels", L"Labels to display"), labelsSettingItemGroupItems, false));

        std::vector<IInspectable> rendererSettingItems =
        {
            AppCoreInt32Item(LocalizationHelper::Localize(L"Texture Resolution", L""), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::Resolution, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Low", L"Low resolution")),
                OptionPair(1, LocalizationHelper::Localize(L"Medium", L"Medium resolution")),
                OptionPair(2, LocalizationHelper::Localize(L"High", L"High resolution"))
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Star Style", L""), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::StarStyle, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Fuzzy Points", L"Star style")),
                OptionPair(1, LocalizationHelper::Localize(L"Points", L"Star style")),
                OptionPair(2, LocalizationHelper::Localize(L"Scaled Discs", L"Star style"))
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Star Colors", L""), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::StarColors, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Classic Colors", L"Star colors option")),
                OptionPair(1, LocalizationHelper::Localize(L"Blackbody D65", L"Star colors option")),
                OptionPair(2, LocalizationHelper::Localize(L"Blackbody (Solar Whitepoint)", L"Star colors option")),
                OptionPair(3, LocalizationHelper::Localize(L"Blackbody (Vega Whitepoint)", L"Star colors option")),
            }), localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Tinted Illumination Saturation", L""), appCore, renderer, CelestiaComponent::CelestiaSettingSingleEntry::TintSaturation, 0.0f, 1.0f, 0.01f, localSettings, LocalizationHelper::Localize(L"Tinted illumination saturation setting is only effective with Blackbody star colors.", L"")),

            AppCoreBooleanItem(LocalizationHelper::Localize(L"Smooth Lines", L"Smooth lines for rendering"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSmoothLines, localSettings),

            AppCoreBooleanItem(LocalizationHelper::Localize(L"Auto Mag", L"Auto mag for star display"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAutoMag, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Ambient Light", L"In setting"), appCore, renderer, CelestiaComponent::CelestiaSettingSingleEntry::AmbientLightLevel, 0.0f, 0.99f, 0.01f, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Faintest Stars", L"Control the faintest star that Celestia should display"), appCore, renderer, CelestiaComponent::CelestiaSettingSingleEntry::FaintestVisible, 3.0f, 12.0f, 1.0f, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Galaxy Brightness", L"Render parameter"), appCore, renderer, CelestiaComponent::CelestiaSettingSingleEntry::GalaxyBrightness, 0.0f, 1.0f, 0.01f, localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Advanced", L"Advanced setting items")),
            AppSettingsBooleanItem(LocalizationHelper::Localize(L"HiDPI", L"HiDPI support in display"), appSettings, AppSettingBooleanEntry::UseFullDPI, localSettings),
            AppSettingsBooleanItem(LocalizationHelper::Localize(L"Anti-aliasing", L""), appSettings, AppSettingBooleanEntry::EnableMSAA, localSettings),
        };

        auto rendererSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        rendererSettingItemGroupItems.ReplaceAll(rendererSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Renderer", L"In settings"), rendererSettingItemGroupItems, true));

        std::vector<IInspectable> regionSettingItems =
        {
            AppCoreInt32Item(LocalizationHelper::Localize(L"Info Display", L"HUD display"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::HudDetail, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"None", L"Empty HUD display")),
                OptionPair(1, LocalizationHelper::Localize(L"Terse", L"Terse HUD display")),
                OptionPair(2, LocalizationHelper::Localize(L"Verbose", L"Verbose HUD display"))
            }), localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Time", L"")),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Time Zone", L""), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::TimeZone, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Local Time", L"")),
                OptionPair(1, LocalizationHelper::Localize(L"UTC", L"")),
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Date Format", L""), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::DateFormat, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Default", L"")),
                OptionPair(1, LocalizationHelper::Localize(L"YYYY MMM DD HH:MM:SS TZ", L"")),
                OptionPair(2, LocalizationHelper::Localize(L"UTC Offset", L""))
            }), localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Region", L"Regions settings")),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Measure Units", L"Measurement system"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::MeasurementSystem, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Metric", L"Metric measurement system")),
                OptionPair(1, LocalizationHelper::Localize(L"Imperial", L"Imperial measurement system"))
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Temperature Scale", L""), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::TemperatureScale, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Kelvin", L"Temperature scale")),
                OptionPair(1, LocalizationHelper::Localize(L"Celsius", L"Temperature scale")),
                OptionPair(2, LocalizationHelper::Localize(L"Fahrenheit", L"Temperature scale"))
            }), localSettings),
            LanguageInt32Item(LocalizationHelper::Localize(L"Language", L"Display language setting"), appSettings, availableLanguages, localSettings),
        };

        auto regionSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        regionSettingItemGroupItems.ReplaceAll(regionSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Time & Region", L"time and region related settings"), regionSettingItemGroupItems, true));

        auto interactionSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        interactionSettingItemGroupItems.ReplaceAll(std::vector<IInspectable>
        {
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Reverse Mouse Wheel", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::EnableReverseWheel, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Ray-Based Dragging", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::EnableRayBasedDragging, localSettings, LocalizationHelper::Localize(L"Dragging behavior based on change of pick rays instead of screen coordinates", L"")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Focus Zooming", L""), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::EnableFocusZooming, localSettings, LocalizationHelper::Localize(L"Zooming behavior keeping the original focus location on screen", L"")),
            AppSettingsDoubleItem(LocalizationHelper::Localize(L"Sensitivity", L"Setting for sensitivity for selecting an object"), appSettings, AppSettingDoubleEntry::PickSensitivity, 1.0, 20.0, 1.0, localSettings, LocalizationHelper::Localize(L"Sensitivity for object selection", L"Notes for the sensitivity setting")),
        });
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Interaction", L"Settings for interaction"), interactionSettingItemGroupItems, true));

        auto cameraSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        cameraSettingItemGroupItems.ReplaceAll(std::vector<IInspectable>
        {
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Align to Surface on Landing", L"Option to align camera to surface when landing"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::EnableAlignCameraToSurfaceOnLand, localSettings),
        });
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Camera", L"Settings for camera control"), cameraSettingItemGroupItems, true));

        auto allActions = single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"None", L"Empty HUD display")),
                OptionPair(1, LocalizationHelper::Localize(L"Travel Faster", L"Game controller action")),
                OptionPair(2, LocalizationHelper::Localize(L"Travel Slower", L"Game controller action")),
                OptionPair(3, LocalizationHelper::Localize(L"Stop", L"Interupt the process of finding eclipse/Set traveling speed to 0")),
                OptionPair(4, LocalizationHelper::Localize(L"Reverse Travel Direction", L"Game controller action")),
                OptionPair(5, LocalizationHelper::Localize(L"Reverse Observer Orientation", L"Game controller action")),
                OptionPair(6, LocalizationHelper::Localize(L"Tap Center", L"Game controller action")),
                OptionPair(7, LocalizationHelper::Localize(L"Go to Object", L"")),
                OptionPair(8, LocalizationHelper::Localize(L"Esc", L"Game controller action")),
                OptionPair(9, LocalizationHelper::Localize(L"Pitch Up", L"Game controller action")),
                OptionPair(10, LocalizationHelper::Localize(L"Pitch Down", L"Game controller action")),
                OptionPair(11, LocalizationHelper::Localize(L"Yaw Left", L"Game controller action")),
                OptionPair(12, LocalizationHelper::Localize(L"Yaw Right", L"Game controller action")),
                OptionPair(13, LocalizationHelper::Localize(L"Roll Left", L"Game controller action")),
                OptionPair(14, LocalizationHelper::Localize(L"Roll Right", L"Game controller action")),
            });
        auto gamepadSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        gamepadSettingItemGroupItems.ReplaceAll(std::vector<IInspectable>
            {
                SettingHeaderItem(LocalizationHelper::Localize(L"Buttons", L"Settings for game controller buttons")),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"A / X", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapA, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"B / Circle", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapB, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"X / Square", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapX, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"Y / Triangle", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapY, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"LB / L1", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapLB, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"LT / L2", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapLT, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"RB / R1", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapRB, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"RT / R2", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapRT, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"D-pad Up", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapDpadUp, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"D-pad Down", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapDpadDown, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"D-pad Left", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapDpadLeft, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"D-pad Right", L"Game controller button"), appSettings, AppSettingInt32Entry::GamepadRemapDpadRight, allActions, localSettings),
                SettingHeaderItem(LocalizationHelper::Localize(L"Thumbsticks", L"Settings for game controller thumbsticks")),
                AppSettingsBooleanItem(LocalizationHelper::Localize(L"Enable Left Thumbstick", L"Setting item to control whether left thumbstick should be enabled"), appSettings, AppSettingBooleanEntry::GamepadEnableLeftThumbstick, localSettings),
                AppSettingsBooleanItem(LocalizationHelper::Localize(L"Enable Right Thumbstick", L"Setting item to control whether right thumbstick should be enabled"), appSettings, AppSettingBooleanEntry::GamepadEnableRightThumbstick, localSettings),
                AppSettingsBooleanItem(LocalizationHelper::Localize(L"Invert Horizontally", L"Invert game controller thumbstick axis horizontally"), appSettings, AppSettingBooleanEntry::GamepadInvertX, localSettings),
                AppSettingsBooleanItem(LocalizationHelper::Localize(L"Invert Vertically", L"Invert game controller thumbstick axis vertically"), appSettings, AppSettingBooleanEntry::GamepadInvertY, localSettings),
            });
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Game Controller", L"Settings for game controller"), gamepadSettingItemGroupItems, false));

        auto advancedSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        advancedSettingItemGroupItems.ReplaceAll(std::vector<IInspectable>
        {
            SettingHeaderItem(LocalizationHelper::Localize(L"Security", L"Security settings title")),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Script System Access Policy", L"Policy for managing lua script's access to the system"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::ScriptSystemAccessPolicy, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Ask", L"Script system access policy option")),
                OptionPair(1, LocalizationHelper::Localize(L"Allow", L"Script system access policy option")),
                OptionPair(2, LocalizationHelper::Localize(L"Deny", L"Script system access policy option"))
            }), localSettings, LocalizationHelper::Localize(L"Lua scripts' access to the file system", L"Note for Script System Access Policy")),
        });
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Advanced", L"Advanced setting items"), advancedSettingItemGroupItems, false));
    }

    void SettingsUserControl::InitializeComponent()
    {
        SettingsUserControlT::InitializeComponent();
        Nav().SelectedItem(itemGroups.GetAt(0));
    }

    Collections::IObservableVector<CelestiaWinUI::SettingsNavigationItemGroup> SettingsUserControl::ItemGroups()
    {
        return itemGroups;
    }

    void SettingsUserControl::NavigationView_SelectionChanged(IInspectable const&, NavigationViewSelectionChangedEventArgs const& args)
    {
        auto selectedItem = args.SelectedItem();
        if (selectedItem == nullptr) return;
        auto itemGroup = selectedItem.try_as<CelestiaWinUI::SettingsNavigationItemGroup>();
        if (itemGroup == nullptr) return;
        ItemGroupSelected(itemGroup);
    }

    void SettingsUserControl::ItemGroupSelected(CelestiaWinUI::SettingsNavigationItemGroup const& itemGroup)
    {
        SettingCommonUserControl userControl{ itemGroup.Items(), itemGroup.ShowRestartHint() };
        Container().Children().Clear();
        Container().Children().Append(userControl);
        Container().SetAlignTopWithPanel(userControl, true);
        Container().SetAlignBottomWithPanel(userControl, true);
        Container().SetAlignLeftWithPanel(userControl, true);
        Container().SetAlignRightWithPanel(userControl, true);
    }
}
