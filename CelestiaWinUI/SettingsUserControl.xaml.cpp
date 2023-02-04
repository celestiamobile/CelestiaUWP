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
            SettingHeaderItem(LocalizationHelper::Localize(L"Objects")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStars, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStars, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Planets"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanets, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Dwarf Planets"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanets, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Moons"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoons, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Minor Moons"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoons, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Asteroids"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroids, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Comets"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowComets, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Spacecraft"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecrafts, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Galaxies"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxies, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Nebulae"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulae, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Globulars"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobulars, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Open Clusters"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusters, localSettings),
            SettingHeaderItem(LocalizationHelper::Localize(L"Features")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Clouds"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudMaps, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Atmospheres"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAtmospheres, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Comet Tails"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometTails, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Planet Rings"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetRings, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Ring Shadows"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowRingShadows, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Eclipse Shadows"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipseShadows, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Cloud Shadows"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudShadows, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Night Lights"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowNightMaps, localSettings),
        };
        auto displaySettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        displaySettingItemGroupItems.ReplaceAll(displaySettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Display"), displaySettingItemGroupItems, false));

        std::vector<IInspectable> guidesSettingItems =
        {
             SettingHeaderItem(LocalizationHelper::Localize(L"Orbits")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Orbits"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Fading Orbits"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowFadingOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Partial Trajectories"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPartialTrajectories, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStellarOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Planets"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Dwarf Planets"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Moons"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Minor Moons"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Asteroids"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Comets"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Spacecraft"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftOrbits, localSettings),

             SettingHeaderItem(LocalizationHelper::Localize(L"Constellations")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Diagrams"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDiagrams, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Labels"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowConstellationLabels, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Labels in Latin"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowLatinConstellationLabels, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Boundaries"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowBoundaries, localSettings),

             SettingHeaderItem(LocalizationHelper::Localize(L"Grids")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Equatorial"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCelestialSphere, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Ecliptic"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipticGrid, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Horizontal"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowHorizonGrid, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Galactic"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalacticGrid, localSettings),

             SettingHeaderItem(LocalizationHelper::Localize(L"Miscellaneous")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Markers"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMarkers, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Ecliptic Line"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEcliptic, localSettings),
        };
        auto guidesSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        guidesSettingItemGroupItems.ReplaceAll(guidesSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Guides"), guidesSettingItemGroupItems, false));

        std::vector<IInspectable> labelsSettingItems =
        {
            SettingHeaderItem(LocalizationHelper::Localize(L"Objects")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStarLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Planets"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Dwarf Planets"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Moons"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Minor Moons"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Asteroids"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Comets"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Spacecraft"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Galaxies"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxyLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Nebulae"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulaLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Globulars"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobularLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Open Clusters"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusterLabels, localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Locations")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Locations"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowLocationLabels, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Minimum Labeled Feature Size"), appCore, renderer, CelestiaComponent::CelestiaSettingSingleEntry::MinimumFeatureSize, 0.0f, 99.0f, 1.0f, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Cities"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCityLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Observatories"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowObservatoryLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Landing Sites"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowLandingSiteLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Montes (Mountains)"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMonsLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Maria (Seas)"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMareLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Craters"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCraterLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Valles (Valleys)"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowVallisLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Terrae (Land masses)"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowTerraLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Volcanoes"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEruptiveCenterLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Other"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOtherLabels, localSettings),
        };
        auto labelsSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        labelsSettingItemGroupItems.ReplaceAll(labelsSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Labels"), labelsSettingItemGroupItems, false));

        std::vector<IInspectable> rendererSettingItems =
        {
            AppCoreInt32Item(LocalizationHelper::Localize(L"Texture Resolution"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::Resolution, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Low")),
                OptionPair(1, LocalizationHelper::Localize(L"Medium")),
                OptionPair(2, LocalizationHelper::Localize(L"High"))
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Star Style"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::StarStyle, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Fuzzy Points")),
                OptionPair(1, LocalizationHelper::Localize(L"Points")),
                OptionPair(2, LocalizationHelper::Localize(L"Scaled Discs"))
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Star Colors"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::StarColors, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Classic Colors")),
                OptionPair(1, LocalizationHelper::Localize(L"Blackbody D65")),
                OptionPair(2, LocalizationHelper::Localize(L"Blackbody (Solar Whitepoint)")),
                OptionPair(3, LocalizationHelper::Localize(L"Blackbody (Vega Whitepoint)")),
            }), localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Tinted Illumination Saturation"), appCore, renderer, CelestiaComponent::CelestiaSettingSingleEntry::TintSaturation, 0.0f, 1.0f, 0.01f, localSettings, LocalizationHelper::Localize(L"Tinted illumination saturation setting is only effective with Blackbody star colors.")),

            AppCoreBooleanItem(LocalizationHelper::Localize(L"Smooth Lines"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSmoothLines, localSettings),

            AppCoreBooleanItem(LocalizationHelper::Localize(L"Auto Mag"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAutoMag, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Ambient Light"), appCore, renderer, CelestiaComponent::CelestiaSettingSingleEntry::AmbientLightLevel, 0.0f, 0.99f, 0.01f, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Faintest Stars"), appCore, renderer, CelestiaComponent::CelestiaSettingSingleEntry::FaintestVisible, 3.0f, 12.0f, 1.0f, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Galaxy Brightness"), appCore, renderer, CelestiaComponent::CelestiaSettingSingleEntry::GalaxyBrightness, 0.0f, 1.0f, 0.01f, localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Advanced")),
            AppSettingsBooleanItem(LocalizationHelper::Localize(L"HiDPI"), appSettings, AppSettingBooleanEntry::UseFullDPI, localSettings),
            AppSettingsBooleanItem(LocalizationHelper::Localize(L"Anti-aliasing"), appSettings, AppSettingBooleanEntry::EnableMSAA, localSettings),
        };

        auto rendererSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        rendererSettingItemGroupItems.ReplaceAll(rendererSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Renderer"), rendererSettingItemGroupItems, true));

        std::vector<IInspectable> regionSettingItems =
        {
            AppCoreInt32Item(LocalizationHelper::Localize(L"Info Display"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::HudDetail, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"None")),
                OptionPair(1, LocalizationHelper::Localize(L"Terse")),
                OptionPair(2, LocalizationHelper::Localize(L"Verbose"))
            }), localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Time")),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Time Zone"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::TimeZone, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Local Time")),
                OptionPair(1, LocalizationHelper::Localize(L"UTC")),
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Date Format"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::DateFormat, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Default")),
                OptionPair(1, LocalizationHelper::Localize(L"YYYY MMM DD HH:MM:SS TZ")),
                OptionPair(2, LocalizationHelper::Localize(L"UTC Offset"))
            }), localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Region")),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Measure Units"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::MeasurementSystem, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Metric")),
                OptionPair(1, LocalizationHelper::Localize(L"Imperial"))
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Temperature Scale"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::TemperatureScale, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Kelvin")),
                OptionPair(1, LocalizationHelper::Localize(L"Celsius")),
                OptionPair(2, LocalizationHelper::Localize(L"Fahrenheit"))
            }), localSettings),
            LanguageInt32Item(LocalizationHelper::Localize(L"Language"), appSettings, availableLanguages, localSettings),
        };

        auto regionSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        regionSettingItemGroupItems.ReplaceAll(regionSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Time & Region"), regionSettingItemGroupItems, true));

        auto interactionSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        interactionSettingItemGroupItems.ReplaceAll(std::vector<IInspectable>
        {
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Reverse Mouse Wheel"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::EnableReverseWheel, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Ray-Based Dragging"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::EnableRayBasedDragging, localSettings, LocalizationHelper::Localize(L"Dragging behavior based on change of pick rays instead of screen coordinates")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Focus Zooming"), appCore, renderer, CelestiaComponent::CelestiaSettingBooleanEntry::EnableFocusZooming, localSettings, LocalizationHelper::Localize(L"Zooming behavior keeping the original focus location on screen")),
            AppSettingsDoubleItem(LocalizationHelper::Localize(L"Sensitivity"), appSettings, AppSettingDoubleEntry::PickSensitivity, 1.0, 20.0, 1.0, localSettings, LocalizationHelper::Localize(L"Sensitivity for object selection")),
        });
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Interaction"), interactionSettingItemGroupItems, true));

        auto allActions = single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"None")),
                OptionPair(1, LocalizationHelper::Localize(L"Travel Faster")),
                OptionPair(2, LocalizationHelper::Localize(L"Travel Slower")),
                OptionPair(3, LocalizationHelper::Localize(L"Stop")),
                OptionPair(4, LocalizationHelper::Localize(L"Reverse Travel Direction")),
                OptionPair(5, LocalizationHelper::Localize(L"Reverse Observer Orientation")),
                OptionPair(6, LocalizationHelper::Localize(L"Tap Center")),
                OptionPair(7, LocalizationHelper::Localize(L"Go to Object")),
                OptionPair(8, LocalizationHelper::Localize(L"Esc")),
                OptionPair(9, LocalizationHelper::Localize(L"Pitch Up")),
                OptionPair(10, LocalizationHelper::Localize(L"Pitch Down")),
                OptionPair(11, LocalizationHelper::Localize(L"Yaw Left")),
                OptionPair(12, LocalizationHelper::Localize(L"Yaw Right")),
                OptionPair(13, LocalizationHelper::Localize(L"Roll Left")),
                OptionPair(14, LocalizationHelper::Localize(L"Roll Right")),
            });
        auto gamepadSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        gamepadSettingItemGroupItems.ReplaceAll(std::vector<IInspectable>
            {
                SettingHeaderItem(LocalizationHelper::Localize(L"Buttons")),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"A / X"), appSettings, AppSettingInt32Entry::GamepadRemapA, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"B / Circle"), appSettings, AppSettingInt32Entry::GamepadRemapB, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"X / Square"), appSettings, AppSettingInt32Entry::GamepadRemapX, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"Y / Triangle"), appSettings, AppSettingInt32Entry::GamepadRemapY, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"LB / L1"), appSettings, AppSettingInt32Entry::GamepadRemapLB, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"LT / L2"), appSettings, AppSettingInt32Entry::GamepadRemapLT, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"RB / R1"), appSettings, AppSettingInt32Entry::GamepadRemapRB, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"RT / R2"), appSettings, AppSettingInt32Entry::GamepadRemapRT, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"D-pad Up"), appSettings, AppSettingInt32Entry::GamepadRemapDpadUp, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"D-pad Down"), appSettings, AppSettingInt32Entry::GamepadRemapDpadDown, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"D-pad Left"), appSettings, AppSettingInt32Entry::GamepadRemapDpadLeft, allActions, localSettings),
                AppSettingsInt32Item(LocalizationHelper::Localize(L"D-pad Right"), appSettings, AppSettingInt32Entry::GamepadRemapDpadRight, allActions, localSettings),
                SettingHeaderItem(LocalizationHelper::Localize(L"Thumbsticks")),
                AppSettingsBooleanItem(LocalizationHelper::Localize(L"Invert Horizontally"), appSettings, AppSettingBooleanEntry::GamepadInvertX, localSettings),
                AppSettingsBooleanItem(LocalizationHelper::Localize(L"Invert Vertically"), appSettings, AppSettingBooleanEntry::GamepadInvertY, localSettings),
            });
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Game Controller"), gamepadSettingItemGroupItems, false));

        auto advancedSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        advancedSettingItemGroupItems.ReplaceAll(std::vector<IInspectable>
        {
            SettingHeaderItem(LocalizationHelper::Localize(L"Security")),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Script System Access Policy"), appCore, renderer, CelestiaComponent::CelestiaSettingInt32Entry::ScriptSystemAccessPolicy, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Ask")),
                OptionPair(1, LocalizationHelper::Localize(L"Allow")),
                OptionPair(2, LocalizationHelper::Localize(L"Deny"))
            }), localSettings, LocalizationHelper::Localize(L"This policy decides whether Lua scripts have access to the files on the system or not.")),
        });
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Advanced"), advancedSettingItemGroupItems, false));

        InitializeComponent();
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
