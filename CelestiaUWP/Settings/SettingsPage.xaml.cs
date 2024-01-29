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
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Display", ""), "display"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Guides", ""), "guides"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Labels", ""), "labels"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Renderer", ""), "renderer"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Time & Region", ""), "region"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Interaction", ""), "interaction"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Game Controller", ""), "gamepad"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Advanced", ""), "advanced"),
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
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowStars, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowPlanets, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowDwarfPlanets, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMoons, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMinorMoons, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowAsteroids, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowComets, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowSpacecrafts, localSettings),
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
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowStellarOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowPlanetOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowDwarfPlanetOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMoonOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMinorMoonOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowAsteroidOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCometOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowSpacecraftOrbits, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Constellations", "")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Diagrams", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowDiagrams, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Labels", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowConstellationLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Labels in Latin", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowLatinConstellationLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Boundaries", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowBoundaries, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Grids", "")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Equatorial", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCelestialSphere, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ecliptic", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowEclipticGrid, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Horizontal", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowHorizonGrid, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galactic", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowGalacticGrid, localSettings),

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
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowStarLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowPlanetLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowDwarfPlanetLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMoonLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMinorMoonLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowAsteroidLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCometLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowSpacecraftLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galaxies", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowGalaxyLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Nebulae", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowNebulaLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Globulars", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowGlobularLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Open Clusters", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowOpenClusterLabels, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Locations", "")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Locations", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowLocationLabels, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Minimum Labeled Feature Size", ""), AppCore, Renderer, CelestiaSettingSingleEntry.MinimumFeatureSize, 0, 99, 1, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Cities", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCityLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Observatories", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowObservatoryLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Landing Sites", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowLandingSiteLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Montes (Mountains)", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMonsLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Maria (Seas)", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowMareLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Craters", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowCraterLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Valles (Valleys)", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowVallisLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Terrae (Land masses)", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowTerraLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Volcanoes", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowEruptiveCenterLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Other", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowOtherLabels, localSettings)
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, false));
            }
            else if (item.Tag == "renderer")
            {
                var items = new SettingBaseItem[]
                {
                    new AppCoreInt32Item(LocalizationHelper.Localize("Texture Resolution", ""), AppCore, Renderer, CelestiaSettingInt32Entry.Resolution, new [] { new OptionPair(0, LocalizationHelper.Localize("Low", "")), new OptionPair(1, LocalizationHelper.Localize("Medium", "")), new OptionPair(2, LocalizationHelper.Localize("High", "")) }, localSettings),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Star Style", ""), AppCore, Renderer, CelestiaSettingInt32Entry.StarStyle, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Fuzzy Points", "")),
                        new OptionPair(1, LocalizationHelper.Localize("Points", "")),
                        new OptionPair(2, LocalizationHelper.Localize("Scaled Discs", "")),
                    }, localSettings),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Star Colors", ""), AppCore, Renderer, CelestiaSettingInt32Entry.StarColors, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Classic Colors", "")),
                        new OptionPair(1, LocalizationHelper.Localize("Blackbody D65", "")),
                        new OptionPair(2, LocalizationHelper.Localize("Blackbody (Solar Whitepoint)", "")),
                        new OptionPair(3, LocalizationHelper.Localize("Blackbody (Vega Whitepoint)", "")),
                    }, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Tinted Illumination Saturation", ""), AppCore, Renderer, CelestiaSettingSingleEntry.TintSaturation, 0, 1, 0.01f, localSettings, LocalizationHelper.Localize("Tinted illumination saturation setting is only effective with Blackbody star colors.", "")),

                    new AppCoreBooleanItem(LocalizationHelper.Localize("Smooth Lines", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowSmoothLines, localSettings),

                    new AppCoreBooleanItem(LocalizationHelper.Localize("Auto Mag", ""), AppCore, Renderer, CelestiaSettingBooleanEntry.ShowAutoMag, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Ambient Light", ""), AppCore, Renderer, CelestiaSettingSingleEntry.AmbientLightLevel, 0, 0.99f, 0.01f, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Faintest Stars", ""), AppCore, Renderer, CelestiaSettingSingleEntry.FaintestVisible, 3, 12, 1, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Galaxy Brightness", ""), AppCore, Renderer, CelestiaSettingSingleEntry.GalaxyBrightness, 0, 1, 0.01f, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Advanced", "")),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("HiDPI", ""), AppSettings, AppSettingBooleanEntry.UseFullDPI, localSettings),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("Anti-aliasing", ""), AppSettings, AppSettingBooleanEntry.EnableMSAA, localSettings),
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, true));
            }
            else if (item.Tag == "region")
            {
                var items = new SettingBaseItem[]
                {
                    new AppCoreInt32Item(LocalizationHelper.Localize("Info Display", ""), AppCore, Renderer, CelestiaSettingInt32Entry.HudDetail, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("None", "")),
                        new OptionPair(1, LocalizationHelper.Localize("Terse", "")),
                        new OptionPair(2, LocalizationHelper.Localize("Verbose", "")),
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

                    new SettingHeaderItem(LocalizationHelper.Localize("Region", "")),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Measure Units", ""), AppCore, Renderer, CelestiaSettingInt32Entry.MeasurementSystem, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Metric", "")),
                        new OptionPair(1, LocalizationHelper.Localize("Imperial", "")),
                    }, localSettings),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Temperature Scale", ""), AppCore, Renderer, CelestiaSettingInt32Entry.TemperatureScale, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Kelvin", "")),
                        new OptionPair(1, LocalizationHelper.Localize("Celsius", "")),
                        new OptionPair(2, LocalizationHelper.Localize("Fahrenheit", "")),
                    }, localSettings),
                    new LanguageInt32Item(LocalizationHelper.Localize("Language", ""), AppSettings, AvailableLanguages, localSettings),
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
                    new AppSettingsDoubleItem(LocalizationHelper.Localize("Sensitivity", ""), AppSettings, AppSettingDoubleEntry.PickSensitivity, 1.0, 20.0, 1.0, localSettings, LocalizationHelper.Localize("Sensitivity for object selection", "")),
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, true));
            }
            else if (item.Tag == "gamepad")
            {
                var actions = new []
                {
                    new OptionPair(0, LocalizationHelper.Localize("None", "")),
                    new OptionPair(1, LocalizationHelper.Localize("Travel Faster", "")),
                    new OptionPair(2, LocalizationHelper.Localize("Travel Slower", "")),
                    new OptionPair(3, LocalizationHelper.Localize("Stop", "")),
                    new OptionPair(4, LocalizationHelper.Localize("Reverse Travel Direction", "")),
                    new OptionPair(5, LocalizationHelper.Localize("Reverse Observer Orientation", "")),
                    new OptionPair(6, LocalizationHelper.Localize("Tap Center", "")),
                    new OptionPair(7, LocalizationHelper.Localize("Go to Object", "")),
                    new OptionPair(8, LocalizationHelper.Localize("Esc", "")),
                    new OptionPair(9, LocalizationHelper.Localize("Pitch Up", "")),
                    new OptionPair(10, LocalizationHelper.Localize("Pitch Down", "")),
                    new OptionPair(11, LocalizationHelper.Localize("Yaw Left", "")),
                    new OptionPair(12, LocalizationHelper.Localize("Yaw Right", "")),
                    new OptionPair(13, LocalizationHelper.Localize("Roll Left", "")),
                    new OptionPair(14, LocalizationHelper.Localize("Roll Right", "")),
                };
                var items = new SettingBaseItem[]
                {
                    new SettingHeaderItem(LocalizationHelper.Localize("Buttons", "")),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("A / X", ""), AppSettings, AppSettingInt32Entry.GamepadRemapA, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("B / Circle", ""), AppSettings, AppSettingInt32Entry.GamepadRemapB, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("X / Square", ""), AppSettings, AppSettingInt32Entry.GamepadRemapX, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("Y / Triangle", ""), AppSettings, AppSettingInt32Entry.GamepadRemapY, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Up", ""), AppSettings, AppSettingInt32Entry.GamepadRemapDpadUp, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Down", ""), AppSettings, AppSettingInt32Entry.GamepadRemapDpadDown, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Left", ""), AppSettings, AppSettingInt32Entry.GamepadRemapDpadLeft, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Right", ""), AppSettings, AppSettingInt32Entry.GamepadRemapDpadRight, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("LB / L1", ""), AppSettings, AppSettingInt32Entry.GamepadRemapLB, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("LT / L2", ""), AppSettings, AppSettingInt32Entry.GamepadRemapLT, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("RB / R1", ""), AppSettings, AppSettingInt32Entry.GamepadRemapRB, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("RT / R2", ""), AppSettings, AppSettingInt32Entry.GamepadRemapRT, actions, localSettings),
                    new SettingHeaderItem(LocalizationHelper.Localize("Thumbsticks", "")),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("Invert Horizontally", ""), AppSettings, AppSettingBooleanEntry.GamepadInvertX, localSettings),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("Invert Vertically", ""), AppSettings, AppSettingBooleanEntry.GamepadInvertY, localSettings),
                };
                Container.Navigate(typeof(SettingsCommonPage), new SettingsParameter(items, false));
            }
            else if (item.Tag == "advanced")
            {
                var items = new SettingBaseItem[]
                {
                    new SettingHeaderItem(LocalizationHelper.Localize("Security", "")),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Script System Access Policy", ""), AppCore, Renderer, CelestiaSettingInt32Entry.ScriptSystemAccessPolicy, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Ask", "")),
                        new OptionPair(1, LocalizationHelper.Localize("Allow", "")),
                        new OptionPair(2, LocalizationHelper.Localize("Deny", "")),
                    }, localSettings, LocalizationHelper.Localize("Lua scripts' access to the file system", "")),
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
