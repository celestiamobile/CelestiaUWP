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
    public sealed partial class SettingsPage : Page, INotifyPropertyChanged
    {
        private Helper.NavigationViewItem[] NavigationItems = new Helper.NavigationViewItem[]
        {
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Display"), "display"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Guides"), "guides"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Labels"), "labels"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Renderer"), "renderer"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Time & Region"), "region"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Game Controller"), "gamepad"),
            new Helper.NavigationViewItem(LocalizationHelper.Localize("Advanced"), "advanced"),
        };

        private CelestiaAppCore AppCore;
        private AppSettings AppSettings;
        private string[] AvailableLanguages;

        public SettingsPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var args = ((CelestiaAppCore, AppSettings, string[]))(e.Parameter);
            AppCore = args.Item1;
            AppSettings = args.Item2;
            AvailableLanguages = args.Item3;

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
                    new SettingHeaderItem(LocalizationHelper.Localize("Objects")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars"), AppCore, CelestiaSettingBooleanEntry.ShowStars, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets"), AppCore, CelestiaSettingBooleanEntry.ShowPlanets, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets"), AppCore, CelestiaSettingBooleanEntry.ShowDwarfPlanets, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMoons, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMinorMoons, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids"), AppCore, CelestiaSettingBooleanEntry.ShowAsteroids, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets"), AppCore, CelestiaSettingBooleanEntry.ShowComets, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft"), AppCore, CelestiaSettingBooleanEntry.ShowSpacecrafts, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galaxies"), AppCore, CelestiaSettingBooleanEntry.ShowGalaxies, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Nebulae"), AppCore, CelestiaSettingBooleanEntry.ShowNebulae, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Globulars"), AppCore, CelestiaSettingBooleanEntry.ShowGlobulars, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Open Clusters"), AppCore, CelestiaSettingBooleanEntry.ShowOpenClusters, localSettings),
                    new SettingHeaderItem(LocalizationHelper.Localize("Features")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Clouds"), AppCore, CelestiaSettingBooleanEntry.ShowCloudMaps, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Atmospheres"), AppCore, CelestiaSettingBooleanEntry.ShowAtmospheres, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comet Tails"), AppCore, CelestiaSettingBooleanEntry.ShowCometTails, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planet Rings"), AppCore, CelestiaSettingBooleanEntry.ShowPlanetRings, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ring Shadows"), AppCore, CelestiaSettingBooleanEntry.ShowRingShadows, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Eclipse Shadows"), AppCore, CelestiaSettingBooleanEntry.ShowEclipseShadows, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Cloud Shadows"), AppCore, CelestiaSettingBooleanEntry.ShowCloudShadows, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Night Lights"), AppCore, CelestiaSettingBooleanEntry.ShowNightMaps, localSettings),
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "guides")
            {
                var items = new SettingBaseItem[]
                {
                    new SettingHeaderItem(LocalizationHelper.Localize("Orbits")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Orbits"), AppCore, CelestiaSettingBooleanEntry.ShowOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Fading Orbits"), AppCore, CelestiaSettingBooleanEntry.ShowFadingOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Partial Trajectories"), AppCore, CelestiaSettingBooleanEntry.ShowPartialTrajectories, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars"), AppCore, CelestiaSettingBooleanEntry.ShowStellarOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets"), AppCore, CelestiaSettingBooleanEntry.ShowPlanetOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets"), AppCore, CelestiaSettingBooleanEntry.ShowDwarfPlanetOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMoonOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMinorMoonOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids"), AppCore, CelestiaSettingBooleanEntry.ShowAsteroidOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets"), AppCore, CelestiaSettingBooleanEntry.ShowCometOrbits, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft"), AppCore, CelestiaSettingBooleanEntry.ShowSpacecraftOrbits, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Constellations")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Diagrams"), AppCore, CelestiaSettingBooleanEntry.ShowDiagrams, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Labels"), AppCore, CelestiaSettingBooleanEntry.ShowConstellationLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Labels in Latin"), AppCore, CelestiaSettingBooleanEntry.ShowLatinConstellationLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Boundaries"), AppCore, CelestiaSettingBooleanEntry.ShowBoundaries, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Grids")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Equatorial"), AppCore, CelestiaSettingBooleanEntry.ShowCelestialSphere, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ecliptic"), AppCore, CelestiaSettingBooleanEntry.ShowEclipticGrid, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Horizontal"), AppCore, CelestiaSettingBooleanEntry.ShowHorizonGrid, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galactic"), AppCore, CelestiaSettingBooleanEntry.ShowGalacticGrid, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Miscellaneous")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Markers"), AppCore, CelestiaSettingBooleanEntry.ShowMarkers, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ecliptic Line"), AppCore, CelestiaSettingBooleanEntry.ShowEcliptic, localSettings)
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "labels")
            {
                var items = new SettingBaseItem[]
                {
                    new SettingHeaderItem(LocalizationHelper.Localize("Objects")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars"), AppCore, CelestiaSettingBooleanEntry.ShowStarLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets"), AppCore, CelestiaSettingBooleanEntry.ShowPlanetLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets"), AppCore, CelestiaSettingBooleanEntry.ShowDwarfPlanetLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMoonLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMinorMoonLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids"), AppCore, CelestiaSettingBooleanEntry.ShowAsteroidLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets"), AppCore, CelestiaSettingBooleanEntry.ShowCometLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft"), AppCore, CelestiaSettingBooleanEntry.ShowSpacecraftLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galaxies"), AppCore, CelestiaSettingBooleanEntry.ShowGalaxyLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Nebulae"), AppCore, CelestiaSettingBooleanEntry.ShowNebulaLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Globulars"), AppCore, CelestiaSettingBooleanEntry.ShowGlobularLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Open Clusters"), AppCore, CelestiaSettingBooleanEntry.ShowOpenClusterLabels, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Locations")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Locations"), AppCore, CelestiaSettingBooleanEntry.ShowLocationLabels, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Minimum Labeled Feature Size"), AppCore, CelestiaSettingSingleEntry.MinimumFeatureSize, 0, 99, 1, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Cities"), AppCore, CelestiaSettingBooleanEntry.ShowCityLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Observatories"), AppCore, CelestiaSettingBooleanEntry.ShowObservatoryLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Landing Sites"), AppCore, CelestiaSettingBooleanEntry.ShowLandingSiteLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Montes (Mountains)"), AppCore, CelestiaSettingBooleanEntry.ShowMonsLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Maria (Seas)"), AppCore, CelestiaSettingBooleanEntry.ShowMareLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Craters"), AppCore, CelestiaSettingBooleanEntry.ShowCraterLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Valles (Valleys)"), AppCore, CelestiaSettingBooleanEntry.ShowVallisLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Terrae (Land masses)"), AppCore, CelestiaSettingBooleanEntry.ShowTerraLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Volcanoes"), AppCore, CelestiaSettingBooleanEntry.ShowEruptiveCenterLabels, localSettings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Other"), AppCore, CelestiaSettingBooleanEntry.ShowOtherLabels, localSettings)
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "renderer")
            {
                var items = new SettingBaseItem[]
                {
                    new AppCoreInt32Item(LocalizationHelper.Localize("Texture Resolution"), AppCore, CelestiaSettingInt32Entry.Resolution, new [] { new OptionPair(0, LocalizationHelper.Localize("Low")), new OptionPair(1, LocalizationHelper.Localize("Medium")), new OptionPair(2, LocalizationHelper.Localize("High")) }, localSettings),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Star Style"), AppCore, CelestiaSettingInt32Entry.StarStyle, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Fuzzy Points")),
                        new OptionPair(1, LocalizationHelper.Localize("Points")),
                        new OptionPair(2, LocalizationHelper.Localize("Scaled Discs")),
                    }, localSettings),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Star Colors"), AppCore, CelestiaSettingInt32Entry.StarColors, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Classic Colors")),
                        new OptionPair(1, LocalizationHelper.Localize("Blackbody D65")),
                        new OptionPair(2, LocalizationHelper.Localize("Blackbody (Solar Whitepoint)")),
                        new OptionPair(3, LocalizationHelper.Localize("Blackbody (Vega Whitepoint)")),
                    }, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Tinted Illumination Saturation"), AppCore, CelestiaSettingSingleEntry.TintSaturation, 0, 1, 0.01f, localSettings, LocalizationHelper.Localize("Tinted illumination saturation setting is only effective with Blackbody star colors.")),

                    new AppCoreBooleanItem(LocalizationHelper.Localize("Smooth Lines"), AppCore, CelestiaSettingBooleanEntry.ShowSmoothLines, localSettings),

                    new AppCoreBooleanItem(LocalizationHelper.Localize("Auto Mag"), AppCore, CelestiaSettingBooleanEntry.ShowAutoMag, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Ambient Light"), AppCore, CelestiaSettingSingleEntry.AmbientLightLevel, 0, 0.99f, 0.01f, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Faintest Stars"), AppCore, CelestiaSettingSingleEntry.FaintestVisible, 3, 12, 1, localSettings),
                    new AppCoreSingleItem(LocalizationHelper.Localize("Galaxy Brightness"), AppCore, CelestiaSettingSingleEntry.GalaxyBrightness, 0, 1, 0.01f, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Advanced")),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("HiDPI"), AppSettings, AppSettingBooleanEntry.UseFullDPI, localSettings),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("Anti-aliasing"), AppSettings, AppSettingBooleanEntry.EnableMSAA, localSettings),
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "region")
            {
                var items = new SettingBaseItem[]
                {
                    new AppCoreInt32Item(LocalizationHelper.Localize("Info Display"), AppCore, CelestiaSettingInt32Entry.HudDetail, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("None")),
                        new OptionPair(1, LocalizationHelper.Localize("Terse")),
                        new OptionPair(2, LocalizationHelper.Localize("Verbose")),
                    }, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Time")),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Time Zone"), AppCore, CelestiaSettingInt32Entry.TimeZone, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Local Time")),
                        new OptionPair(1, LocalizationHelper.Localize("UTC")),
                    }, localSettings),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Date Format"), AppCore, CelestiaSettingInt32Entry.DateFormat, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Default")),
                        new OptionPair(1, LocalizationHelper.Localize("YYYY MMM DD HH:MM:SS TZ")),
                        new OptionPair(2, LocalizationHelper.Localize("UTC Offset")),
                    }, localSettings),

                    new SettingHeaderItem(LocalizationHelper.Localize("Region")),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Measure Units"), AppCore, CelestiaSettingInt32Entry.MeasurementSystem, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Metric")),
                        new OptionPair(1, LocalizationHelper.Localize("Imperial")),
                    }, localSettings),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Temperature Scale"), AppCore, CelestiaSettingInt32Entry.TemperatureScale, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Kelvin")),
                        new OptionPair(1, LocalizationHelper.Localize("Celsius")),
                        new OptionPair(2, LocalizationHelper.Localize("Fahrenheit")),
                    }, localSettings),
                    new LanguageInt32Item(LocalizationHelper.Localize("Language"), AppSettings, AvailableLanguages, localSettings),
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "gamepad")
            {
                var actions = new []
                {
                    new OptionPair(0, LocalizationHelper.Localize("None")),
                    new OptionPair(1, LocalizationHelper.Localize("Travel Faster")),
                    new OptionPair(2, LocalizationHelper.Localize("Travel Slower")),
                    new OptionPair(3, LocalizationHelper.Localize("Stop")),
                    new OptionPair(4, LocalizationHelper.Localize("Reverse Travel Direction")),
                    new OptionPair(5, LocalizationHelper.Localize("Reverse Observer Orientation")),
                    new OptionPair(6, LocalizationHelper.Localize("Tap Center")),
                    new OptionPair(7, LocalizationHelper.Localize("Go to Object")),
                    new OptionPair(8, LocalizationHelper.Localize("Esc")),
                    new OptionPair(9, LocalizationHelper.Localize("Pitch Up")),
                    new OptionPair(10, LocalizationHelper.Localize("Pitch Down")),
                    new OptionPair(11, LocalizationHelper.Localize("Yaw Left")),
                    new OptionPair(12, LocalizationHelper.Localize("Yaw Right")),
                    new OptionPair(13, LocalizationHelper.Localize("Roll Left")),
                    new OptionPair(14, LocalizationHelper.Localize("Roll Right")),
                };
                var items = new SettingBaseItem[]
                {
                    new SettingHeaderItem(LocalizationHelper.Localize("Buttons")),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("A / X"), AppSettings, AppSettingInt32Entry.GamepadRemapA, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("B / Circle"), AppSettings, AppSettingInt32Entry.GamepadRemapB, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("X / Square"), AppSettings, AppSettingInt32Entry.GamepadRemapX, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("Y / Triangle"), AppSettings, AppSettingInt32Entry.GamepadRemapY, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Up"), AppSettings, AppSettingInt32Entry.GamepadRemapDpadUp, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Down"), AppSettings, AppSettingInt32Entry.GamepadRemapDpadDown, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Left"), AppSettings, AppSettingInt32Entry.GamepadRemapDpadLeft, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("D-pad Right"), AppSettings, AppSettingInt32Entry.GamepadRemapDpadRight, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("LB / L1"), AppSettings, AppSettingInt32Entry.GamepadRemapLB, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("LT / L2"), AppSettings, AppSettingInt32Entry.GamepadRemapLT, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("RB / R1"), AppSettings, AppSettingInt32Entry.GamepadRemapRB, actions, localSettings),
                    new AppSettingsInt32Item(LocalizationHelper.Localize("RT / R2"), AppSettings, AppSettingInt32Entry.GamepadRemapRT, actions, localSettings),
                    new SettingHeaderItem(LocalizationHelper.Localize("Thumbsticks")),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("Invert Horizontally"), AppSettings, AppSettingBooleanEntry.GamepadInvertX, localSettings),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("Invert Vertically"), AppSettings, AppSettingBooleanEntry.GamepadInvertY, localSettings),
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "advanced")
            {
                var items = new SettingBaseItem[]
                {
                    new SettingHeaderItem(LocalizationHelper.Localize("Security")),
                    new AppCoreInt32Item(LocalizationHelper.Localize("Script System Access Policy"), AppCore, CelestiaSettingInt32Entry.ScriptSystemAccessPolicy, new []
                    {
                        new OptionPair(0, LocalizationHelper.Localize("Ask")),
                        new OptionPair(1, LocalizationHelper.Localize("Allow")),
                        new OptionPair(2, LocalizationHelper.Localize("Deny")),
                    }, localSettings, LocalizationHelper.Localize("This policy decides whether Lua scripts have access to the files on the system or not.")),
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
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
