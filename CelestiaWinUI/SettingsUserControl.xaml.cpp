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
    SettingsNavigationItemGroup::SettingsNavigationItemGroup(hstring const& title, Collections::IObservableVector<IInspectable> const& items) : title(title), items(items) {};

    hstring SettingsNavigationItemGroup::Title()
    {
        return title;
    }

    Collections::IObservableVector<IInspectable> SettingsNavigationItemGroup::Items()
    {
        return items;
    }

    SettingsUserControl::SettingsUserControl(CelestiaAppCore const& appCore, AppSettings const& appSettings, Windows::Storage::ApplicationDataContainer const& localSettings, Collections::IVector<hstring> const& availableLanguages) : appCore(appCore), appSettings(appSettings)
    {
        itemGroups = single_threaded_observable_vector<CelestiaWinUI::SettingsNavigationItemGroup>();
        std::vector<IInspectable> displaySettingItems =
        {
            SettingHeaderItem(LocalizationHelper::Localize(L"Objects")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStars, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStars, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Planets"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanets, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Dwarf Planets"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanets, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Moons"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoons, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Minor Moons"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoons, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Asteroids"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroids, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Comets"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowComets, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Spacecraft"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecrafts, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Galaxies"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxies, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Nebulae"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulae, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Globulars"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobulars, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Open Clusters"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusters, localSettings),
            SettingHeaderItem(LocalizationHelper::Localize(L"Features")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Clouds"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudMaps, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Atmospheres"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAtmospheres, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Comet Tails"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometTails, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Planet Rings"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetRings, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Ring Shadows"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowRingShadows, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Eclipse Shadows"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipseShadows, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Cloud Shadows"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCloudShadows, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Night Lights"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowNightMaps, localSettings),
        };
        auto displaySettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        displaySettingItemGroupItems.ReplaceAll(displaySettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Display"), displaySettingItemGroupItems));

        std::vector<IInspectable> guidesSettingItems =
        {
             SettingHeaderItem(LocalizationHelper::Localize(L"Orbits")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Orbits"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Fading Orbits"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowFadingOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Partial Trajectories"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPartialTrajectories, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStellarOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Planets"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Dwarf Planets"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Moons"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Minor Moons"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Asteroids"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Comets"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometOrbits, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Spacecraft"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftOrbits, localSettings),

             SettingHeaderItem(LocalizationHelper::Localize(L"Constellations")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Diagrams"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDiagrams, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Labels"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowConstellationLabels, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Labels in Latin"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowLatinConstellationLabels, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Boundaries"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowBoundaries, localSettings),

             SettingHeaderItem(LocalizationHelper::Localize(L"Grids")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Equatorial"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCelestialSphere, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Ecliptic"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEclipticGrid, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Horizontal"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowHorizonGrid, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Galactic"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalacticGrid, localSettings),

             SettingHeaderItem(LocalizationHelper::Localize(L"Miscellaneous")),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Markers"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMarkers, localSettings),
             AppCoreBooleanItem(LocalizationHelper::Localize(L"Ecliptic Line"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEcliptic, localSettings),
        };
        auto guidesSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        guidesSettingItemGroupItems.ReplaceAll(guidesSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Guides"), guidesSettingItemGroupItems));

        std::vector<IInspectable> labelsSettingItems =
        {
            SettingHeaderItem(LocalizationHelper::Localize(L"Objects")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Stars"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowStarLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Planets"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowPlanetLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Dwarf Planets"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowDwarfPlanetLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Moons"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMoonLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Minor Moons"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMinorMoonLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Asteroids"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAsteroidLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Comets"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCometLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Spacecraft"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSpacecraftLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Galaxies"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGalaxyLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Nebulae"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowNebulaLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Globulars"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowGlobularLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Open Clusters"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOpenClusterLabels, localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Locations")),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Show Locations"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowLocationLabels, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Minimum Labeled Feature Size"), appCore, CelestiaComponent::CelestiaSettingSingleEntry::MinimumFeatureSize, 0.0f, 99.0f, 1.0f, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Cities"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCityLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Observatories"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowObservatoryLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Landing Sites"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowLandingSiteLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Montes (Mountains)"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMonsLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Maria (Seas)"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowMareLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Craters"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowCraterLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Valles (Valleys)"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowVallisLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Terrae (Land masses)"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowTerraLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Volcanoes"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowEruptiveCenterLabels, localSettings),
            AppCoreBooleanItem(LocalizationHelper::Localize(L"Other"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowOtherLabels, localSettings),
        };
        auto labelsSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        labelsSettingItemGroupItems.ReplaceAll(labelsSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Labels"), labelsSettingItemGroupItems));

        std::vector<IInspectable> rendererSettingItems =
        {
            AppCoreInt32Item(LocalizationHelper::Localize(L"Texture Resolution"), appCore, CelestiaComponent::CelestiaSettingInt32Entry::Resolution, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Low")),
                OptionPair(1, LocalizationHelper::Localize(L"Medium")),
                OptionPair(2, LocalizationHelper::Localize(L"High"))
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Star Style"), appCore, CelestiaComponent::CelestiaSettingInt32Entry::StarStyle, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Fuzzy Points")),
                OptionPair(1, LocalizationHelper::Localize(L"Points")),
                OptionPair(2, LocalizationHelper::Localize(L"Scaled Discs"))
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Star Colors"), appCore, CelestiaComponent::CelestiaSettingInt32Entry::StarColors, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Classic Colors")),
                OptionPair(1, LocalizationHelper::Localize(L"Blackbody D65")),
            }), localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Tinted Illumination Saturation"), appCore, CelestiaComponent::CelestiaSettingSingleEntry::TintSaturation, 0.0f, 1.0f, 0.01f, localSettings, LocalizationHelper::Localize(L"Tinted illuminaton saturation setting is only effective with Blackbody D65 star colors.")),

            AppCoreBooleanItem(LocalizationHelper::Localize(L"Smooth Lines"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowSmoothLines, localSettings),

            AppCoreBooleanItem(LocalizationHelper::Localize(L"Auto Mag"), appCore, CelestiaComponent::CelestiaSettingBooleanEntry::ShowAutoMag, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Ambient Light"), appCore, CelestiaComponent::CelestiaSettingSingleEntry::AmbientLightLevel, 0.0f, 0.99f, 0.01f, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Faintest Stars"), appCore, CelestiaComponent::CelestiaSettingSingleEntry::FaintestVisible, 3.0f, 12.0f, 1.0f, localSettings),
            AppCoreSingleItem(LocalizationHelper::Localize(L"Galaxy Brightness"), appCore, CelestiaComponent::CelestiaSettingSingleEntry::GalaxyBrightness, 0.0f, 1.0f, 0.01f, localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Advanced")),
            AppSettingsBooleanItem(LocalizationHelper::Localize(L"HiDPI"), appSettings, AppSettingBooleanEntry::UseFullDPI, localSettings),
            AppSettingsBooleanItem(LocalizationHelper::Localize(L"Anti-aliasing"), appSettings, AppSettingBooleanEntry::EnableMSAA, localSettings),
        };

        auto rendererSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        rendererSettingItemGroupItems.ReplaceAll(rendererSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Renderer"), rendererSettingItemGroupItems));

        std::vector<IInspectable> regionSettingItems =
        {
            AppCoreInt32Item(LocalizationHelper::Localize(L"Info Display"), appCore, CelestiaComponent::CelestiaSettingInt32Entry::HudDetail, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"None")),
                OptionPair(1, LocalizationHelper::Localize(L"Terse")),
                OptionPair(2, LocalizationHelper::Localize(L"Verbose"))
            }), localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Time")),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Time Zone"), appCore, CelestiaComponent::CelestiaSettingInt32Entry::TimeZone, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Local Time")),
                OptionPair(1, LocalizationHelper::Localize(L"UTC")),
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Date Format"), appCore, CelestiaComponent::CelestiaSettingInt32Entry::DateFormat, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Default")),
                OptionPair(1, LocalizationHelper::Localize(L"YYYY MMM DD HH:MM:SS TZ")),
                OptionPair(2, LocalizationHelper::Localize(L"UTC Offset"))
            }), localSettings),

            SettingHeaderItem(LocalizationHelper::Localize(L"Region")),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Measure Units"), appCore, CelestiaComponent::CelestiaSettingInt32Entry::MeasurementSystem, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Metric")),
                OptionPair(1, LocalizationHelper::Localize(L"Imperial"))
            }), localSettings),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Temperature Scale"), appCore, CelestiaComponent::CelestiaSettingInt32Entry::TemperatureScale, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Kelvin")),
                OptionPair(1, LocalizationHelper::Localize(L"Celsius")),
                OptionPair(2, LocalizationHelper::Localize(L"Fahrenheit"))
            }), localSettings),
            LanguageInt32Item(LocalizationHelper::Localize(L"Language"), appSettings, availableLanguages, localSettings),
        };

        auto regionSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        regionSettingItemGroupItems.ReplaceAll(regionSettingItems);
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Time & Region"), regionSettingItemGroupItems));

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
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Game Controller"), gamepadSettingItemGroupItems));

        auto advancedSettingItemGroupItems = single_threaded_observable_vector<IInspectable>();
        advancedSettingItemGroupItems.ReplaceAll(std::vector<IInspectable>
        {
            SettingHeaderItem(LocalizationHelper::Localize(L"Security")),
            AppCoreInt32Item(LocalizationHelper::Localize(L"Script System Access Policy"), appCore, CelestiaComponent::CelestiaSettingInt32Entry::ScriptSystemAccessPolicy, single_threaded_vector<OptionPair>
            ({
                OptionPair(0, LocalizationHelper::Localize(L"Ask")),
                OptionPair(1, LocalizationHelper::Localize(L"Allow")),
                OptionPair(2, LocalizationHelper::Localize(L"Deny"))
            }), localSettings, LocalizationHelper::Localize(L"This policy decides whether Lua scripts have access to the files on the system or not.")),
        });
        itemGroups.Append(CelestiaWinUI::SettingsNavigationItemGroup(LocalizationHelper::Localize(L"Advanced"), advancedSettingItemGroupItems));

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
        SettingCommonUserControl userControl{ itemGroup.Items() };
        Container().Children().Clear();
        Container().Children().Append(userControl);
        Container().SetAlignTopWithPanel(userControl, true);
        Container().SetAlignBottomWithPanel(userControl, true);
        Container().SetAlignLeftWithPanel(userControl, true);
        Container().SetAlignRightWithPanel(userControl, true);
    }
}
