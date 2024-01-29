//
// SettingsPage.xaml.cs
//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software, you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version 2
// of the License, or (at your option) any later version.
//

using CelestiaAppComponent;
using CelestiaComponent;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

using NavigationView = Microsoft.UI.Xaml.Controls.NavigationView;
using NavigationViewSelectionChangedEventArgs = Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs;

namespace CelestiaUWP.Settings
{
    public class SettingsArgs
    {
        public CelestiaAppCore AppCore;
        public CelestiaRenderer Renderer;
        public AppSettings AppSettings;
        public string[] AvailableLanguages;

        public SettingsArgs(CelestiaAppCore appCore, CelestiaRenderer renderer, AppSettings appSettings, string[] availableLanguages)
        {
            AppCore = appCore;
            Renderer = renderer;
            AppSettings = appSettings;
            AvailableLanguages = availableLanguages;
        }
    }

    public sealed partial class SettingsPage : Page, INotifyPropertyChanged
    {
        private Helper.NavigationViewItem[] NavigationItems = new Helper.NavigationViewItem[]
        {
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Display", "Display settings"), "display"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Guides", "Grids, labels, orbits, markers"), "guides"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Labels", "Labels to display"), "labels"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Renderer", "In settings"), "renderer"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Time & Region", "time and region related settings"), "region"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Interaction", "Settings for interaction"), "interaction"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Game Controller", "Settings for game controller"), "gamepad"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Advanced", "Advanced setting items"), "advanced"),
        };

        private CelestiaAppCore AppCore;
        private CelestiaRenderer Renderer;
        private AppSettings AppSettings;
        private string[] AvailableLanguages;

        public SettingsPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var args = (SettingsArgs)e.Parameter;
            AppCore = args.AppCore;
            Renderer = args.Renderer;
            AppSettings = args.AppSettings;
            AvailableLanguages = args.AvailableLanguages;

            Nav.SelectedItem = NavigationItems[0];
        }

        private void Nav_SelectionChanged(NavigationView sender, NavigationViewSelectionChangedEventArgs args)
        {
            if (args.SelectedItem == null) return;

            var item = (Helper.NavigationViewItem)args.SelectedItem;
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            if (item.Tag == "display")
            {
                var items = new SettingBaseItem[]
                {
                    new SettingHeaderItem(LocalizationHelper.Localize("Objects", "")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars", "Tab for stars in Star Browser"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowStars, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowPlanets, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowDwarfPlanets, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMoons, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMinorMoons, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowAsteroids, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowComets, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft", "Plural"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowSpacecrafts, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galaxies", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowGalaxies, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Nebulae", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowNebulae, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Globulars", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowGlobulars, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Open Clusters", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowOpenClusters, localSettings),
                    new SettingHeaderItem(LocalizationHelper.Localize("Features", "")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Clouds", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCloudMaps, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Atmospheres", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowAtmospheres, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comet Tails", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCometTails, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planet Rings", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowPlanetRings, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ring Shadows", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowRingShadows, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Eclipse Shadows", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowEclipseShadows, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Cloud Shadows", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCloudShadows, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Night Lights", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowNightMaps, localSettings),
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, false));
            }
            else if (item.Tag == "guides")
            {
                var items = new SettingBaseItem[]
                {
                    new SettingHeaderItem(LocalizationHelper.Localize("Orbits", "")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Orbits", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Fading Orbits", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowFadingOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Partial Trajectories", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowPartialTrajectories, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars", "Tab for stars in Star Browser"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowStellarOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowPlanetOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowDwarfPlanetOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMoonOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMinorMoonOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowAsteroidOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCometOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft", "Plural"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowSpacecraftOrbits, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Constellations", "")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Diagrams", "Show constellation diagrams"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowDiagrams, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Labels", "Constellation labels"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowConstellationLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Labels in Latin", "Constellation labels in Latin"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowLatinConstellationLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Boundaries", "Show constellation boundaries"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowBoundaries, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Grids", "")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Equatorial", "Grids"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCelestialSphere, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ecliptic", "Grids"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowEclipticGrid, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Horizontal", "Grids"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowHorizonGrid, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galactic", "Grids"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowGalacticGrid, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Miscellaneous", "")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Markers", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMarkers, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ecliptic Line", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowEcliptic, localSettings)
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, false));
            }
            else if (item.Tag == "labels")
            {
                var items = new SettingBaseItem[]
                {
                    new SettingHeaderItem(LocalizationHelper.Localize("Objects", "")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars", "Tab for stars in Star Browser"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowStarLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowPlanetLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowDwarfPlanetLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMoonLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMinorMoonLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowAsteroidLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCometLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft", "Plural"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowSpacecraftLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galaxies", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowGalaxyLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Nebulae", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowNebulaLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Globulars", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowGlobularLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Open Clusters", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowOpenClusterLabels, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Locations", "Location labels to display")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Locations", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowLocationLabels, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Minimum Labeled Feature Size", "Minimum feature size that we should display a label for"), AppCore, Renderer, CelestiaSettingSingleEntry.MinimumFeatureSize, 0, 99, 1, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Cities", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCityLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Observatories", "Location labels"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowObservatoryLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Landing Sites", "Location labels"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowLandingSiteLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Montes (Mountains)", "Location labels"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMonsLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Maria (Seas)", "Location labels"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMareLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Craters", "Location labels"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCraterLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Valles (Valleys)", "Location labels"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowVallisLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Terrae (Land masses)", "Location labels"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowTerraLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Volcanoes", "Location labels"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowEruptiveCenterLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Other", "Other location labels; Android/iOS, Other objects to choose from in Eclipse Finder"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowOtherLabels, localSettings)
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, false));
            }
            else if (item.Tag == "renderer")
            {
                var items = new SettingBaseItem[]
                {
                    new AppCoreInt32Item(LocalizationHelper.Localize("Texture Resolution", ""), AppCore, Renderer, CelestiaSettingInt32Entry.Resolution, new [] { new OptionPair(0, LocalizationHelper.Localize("Low", "Low resolution")), new OptionPair(1, LocalizationHelper.Localize("Medium", "Medium resolution")), new OptionPair(2, LocalizationHelper.Localize("High", "High resolution")) }, localSettings),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Star Style", ""), AppCore, Renderer, CelestiaSettingInt32Entry.StarStyle, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Fuzzy Points", "Star style")),
                        new OptionPair(1, LocalizationHelper.Localize("Points", "Star style")),
                        new OptionPair(2, LocalizationHelper.Localize("Scaled Discs", "Star style")),
                    }, localSettings),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Star Colors", ""), AppCore, Renderer, CelestiaSettingInt32Entry.StarColors, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Classic Colors", "Star colors option")),
                        new OptionPair(1, LocalizationHelper.Localize("Blackbody D65", "Star colors option")),
                        new OptionPair(2, LocalizationHelper.Localize("Blackbody (Solar Whitepoint)", "Star colors option")),
                        new OptionPair(3, LocalizationHelper.Localize("Blackbody (Vega Whitepoint)", "Star colors option")),
                    }, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Tinted Illumination Saturation", ""), AppCore, Renderer, CelestiaSettingSingleEntry.TintSaturation, 0, 1, 0.01f, localSettings, LocalizationHelper.Localize("Tinted illumination saturation setting is only effective with Blackbody star colors.", "")),

                    new AppCoreBooleanItem(LocalizationHelper.Localize("Smooth Lines", "Smooth lines for rendering"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowSmoothLines, localSettings),

                    new AppCoreBooleanItem(LocalizationHelper.Localize("Auto Mag", "Auto mag for star display"), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowAutoMag, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Ambient Light", "In setting"), AppCore, Renderer, CelestiaSettingSingleEntry.AmbientLightLevel, 0, 0.99f, 0.01f, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Faintest Stars", "Control the faintest star that Celestia should display"), AppCore, Renderer, CelestiaSettingSingleEntry.FaintestVisible, 3, 12, 1, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Galaxy Brightness", "Render parameter"), AppCore, Renderer, CelestiaSettingSingleEntry.GalaxyBrightness, 0, 1, 0.01f, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Advanced", "Advanced setting items")),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("HiDPI", "HiDPI support in display"), AppSettings, AppSettingBooleanEntry.UseFullDPI, localSettings),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("Anti-aliasing", ""), AppSettings, AppSettingBooleanEntry.EnableMSAA, localSettings),
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, true));
            }
            else if (item.Tag == "region")
            {
                var items = new SettingBaseItem[]
                {
                    new AppCoreInt32Item(LocalizationHelper.Localize("Info Display", "HUD display"), AppCore, Renderer, CelestiaSettingInt32Entry.HudDetail, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("None", "Empty HUD display")),
                        new OptionPair(1, LocalizationHelper.Localize("Terse", "Terse HUD display")),
                        new OptionPair(2, LocalizationHelper.Localize("Verbose", "Verbose HUD display")),
                    }, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Time", "")),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Time Zone", ""), AppCore, Renderer, CelestiaSettingInt32Entry.TimeZone, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Local Time", "")),
                        new OptionPair(1, LocalizationHelper.Localize("UTC", "")),
                    }, localSettings),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Date Format", ""), AppCore, Renderer, CelestiaSettingInt32Entry.DateFormat, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Default", "")),
                        new OptionPair(1, LocalizationHelper.Localize("YYYY MMM DD HH:MM:SS TZ", "")),
                        new OptionPair(2, LocalizationHelper.Localize("UTC Offset", "")),
                    }, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Region", "Regions settings")),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Measure Units", "Measurement system"), AppCore, Renderer, CelestiaSettingInt32Entry.MeasurementSystem, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Metric", "Metric measurement system")),
                        new OptionPair(1, LocalizationHelper.Localize("Imperial", "Imperial measurement system")),
                    }, localSettings),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Temperature Scale", ""), AppCore, Renderer, CelestiaSettingInt32Entry.TemperatureScale, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Kelvin", "Temperature scale")),
                        new OptionPair(1, LocalizationHelper.Localize("Celsius", "Temperature scale")),
                        new OptionPair(2, LocalizationHelper.Localize("Fahrenheit", "Temperature scale")),
                    }, localSettings),
                    new LanguageInt32Item(LocalizationHelper.Localize("Language", "Display language setting"), AppSettings, AvailableLanguages, localSettings),
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, true));
            }
            else if (item.Tag == "interaction")
            {
                var items = new SettingBaseItem[]
                {
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Reverse Mouse Wheel", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.EnableReverseWheel, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ray-Based Dragging", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.EnableRayBasedDragging, localSettings, LocalizationHelper.Localize("Dragging behavior based on change of pick rays instead of screen coordinates", "")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Focus Zooming", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.EnableFocusZooming, localSettings, LocalizationHelper.Localize("Zooming behavior keeping the original focus location on screen", "")),
                    new AppSettingsDoubleItem(LocalizationHelper.Localize("Sensitivity", "Setting for sensitivity for selecting an object"), AppSettings, AppSettingDoubleEntry.PickSensitivity, 1.0, 20.0, 1.0, localSettings, LocalizationHelper.Localize("Sensitivity for object selection", "Notes for the sensitivity setting")),
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, true));
            }
            else if (item.Tag == "gamepad")
            {
                var actions = new []
                {
                    new OptionPair(0, LocalizationHelper.Localize("None", "Empty HUD display")),
                    new OptionPair(1, LocalizationHelper.Localize("Travel Faster", "Game controller action")),
                    new OptionPair(2, LocalizationHelper.Localize("Travel Slower", "Game controller action")),
                    new OptionPair(3, LocalizationHelper.Localize("Stop", "Interupt the process of finding eclipse/Set traveling speed to 0")),
                    new OptionPair(4, LocalizationHelper.Localize("Reverse Travel Direction", "Game controller action")),
                    new OptionPair(5, LocalizationHelper.Localize("Reverse Observer Orientation", "Game controller action")),
                    new OptionPair(6, LocalizationHelper.Localize("Tap Center", "Game controller action")),
                    new OptionPair(7, LocalizationHelper.Localize("Go to Object", "")),
                    new OptionPair(8, LocalizationHelper.Localize("Esc", "Game controller action")),
                    new OptionPair(9, LocalizationHelper.Localize("Pitch Up", "Game controller action")),
                    new OptionPair(10, LocalizationHelper.Localize("Pitch Down", "Game controller action")),
                    new OptionPair(11, LocalizationHelper.Localize("Yaw Left", "Game controller action")),
                    new OptionPair(12, LocalizationHelper.Localize("Yaw Right", "Game controller action")),
                    new OptionPair(13, LocalizationHelper.Localize("Roll Left", "Game controller action")),
                    new OptionPair(14, LocalizationHelper.Localize("Roll Right", "Game controller action")),
                };
                var items = new SettingBaseItem[]
                {
                    new SettingHeaderItem(LocalizationHelper.Localize("Buttons", "Settings for game controller buttons")),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("A / X", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapA, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("B / Circle", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapB, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("X / Square", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapX, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("Y / Triangle", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapY, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Up", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapDpadUp, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Down", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapDpadDown, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Left", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapDpadLeft, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Right", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapDpadRight, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("LB / L1", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapLB, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("LT / L2", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapLT, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("RB / R1", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapRB, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("RT / R2", "Game controller button"), AppSettings, AppSettingInt32Entry.GamepadRemapRT, actions, localSettings),
                    new SettingHeaderItem(LocalizationHelper.Localize("Thumbsticks", "Settings for game controller thumbsticks")),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("Invert Horizontally", "Invert game controller thumbstick axis horizontally"), AppSettings, AppSettingBooleanEntry.GamepadInvertX, localSettings),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("Invert Vertically", "Invert game controller thumbstick axis vertically"), AppSettings, AppSettingBooleanEntry.GamepadInvertY, localSettings),
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, false));
            }
            else if (item.Tag == "advanced")
            {
                var items = new SettingBaseItem[]
                {
                    new SettingHeaderItem(LocalizationHelper.Localize("Security", "Security settings title")),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Script System Access Policy", "Policy for managing lua script's access to the system"), AppCore, Renderer, CelestiaSettingInt32Entry.ScriptSystemAccessPolicy, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Ask", "Script system access policy option")),
                        new OptionPair(1, LocalizationHelper.Localize("Allow", "Script system access policy option")),
                        new OptionPair(2, LocalizationHelper.Localize("Deny", "Script system access policy option")),
                    }, localSettings, LocalizationHelper.Localize("Lua scripts' access to the file system", "Note for Script System Access Policy")),
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, false));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void OnPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}
