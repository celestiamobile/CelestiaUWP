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

using CelestiaComponent;
using CelestiaUWP.Helper;
using System.ComponentModel;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

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

            if (item.Tag == "display")
            {
                var items = new SettingsCommonItem[]
                {
                    new SettingsHeaderItem(LocalizationHelper.Localize("Objects")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars"), AppCore, "ShowStars"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets"), AppCore, "ShowPlanets"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets"), AppCore, "ShowDwarfPlanets"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons"), AppCore, "ShowMoons"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons"), AppCore, "ShowMinorMoons"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids"), AppCore, "ShowAsteroids"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets"), AppCore, "ShowComets"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft"), AppCore, "ShowSpacecrafts"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galaxies"), AppCore, "ShowGalaxies"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Nebulae"), AppCore, "ShowNebulae"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Globulars"), AppCore, "ShowGlobulars"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Open Clusters"), AppCore, "ShowOpenClusters"),
                    new SettingsHeaderItem(LocalizationHelper.Localize("Features")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Clouds"), AppCore, "ShowCloudMaps"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Atmospheres"), AppCore, "ShowAtmospheres"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comet Tails"), AppCore, "ShowCometTails"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planet Rings"), AppCore, "ShowPlanetRings"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ring Shadows"), AppCore, "ShowRingShadows"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Eclipse Shadows"), AppCore, "ShowEclipseShadows"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Cloud Shadows"), AppCore, "ShowCloudShadows"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Night Lights"), AppCore, "ShowNightMaps"),
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "guides")
            {
                var items = new SettingsCommonItem[]
                {
                    new SettingsHeaderItem(LocalizationHelper.Localize("Orbits")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Orbits"), AppCore, "ShowOrbits"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Fading Orbits"), AppCore, "ShowFadingOrbits"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Partial Trajectories"), AppCore, "ShowPartialTrajectories"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars"), AppCore, "ShowStellarOrbits"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets"), AppCore, "ShowPlanetOrbits"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets"), AppCore, "ShowDwarfPlanetOrbits"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons"), AppCore, "ShowMoonOrbits"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons"), AppCore, "ShowMinorMoonOrbits"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids"), AppCore, "ShowAsteroidOrbits"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets"), AppCore, "ShowCometOrbits"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft"), AppCore, "ShowSpacecraftOrbits"),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Constellations")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Diagrams"), AppCore, "ShowDiagrams"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Constellation Labels"), AppCore, "ShowConstellationLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Constellations in Latin"), AppCore, "ShowLatinConstellationLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Boundaries"), AppCore, "ShowBoundaries"),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Grids")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Equatorial"), AppCore, "ShowCelestialSphere"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ecliptic"), AppCore, "ShowEclipticGrid"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Horizontal"), AppCore, "ShowHorizonGrid"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galactic"), AppCore, "ShowGalacticGrid"),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Miscellaneous")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Markers"), AppCore, "ShowMarkers"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ecliptic Line"), AppCore, "ShowEcliptic")
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "labels")
            {
                var items = new SettingsCommonItem[]
                {
                    new SettingsHeaderItem(LocalizationHelper.Localize("Objects")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars"), AppCore, "ShowStarLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets"), AppCore, "ShowPlanetLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets"), AppCore, "ShowDwarfPlanetLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons"), AppCore, "ShowMoonLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons"), AppCore, "ShowMinorMoonLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids"), AppCore, "ShowAsteroidLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets"), AppCore, "ShowCometLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft"), AppCore, "ShowSpacecraftLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galaxies"), AppCore, "ShowGalaxyLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Nebulae"), AppCore, "ShowNebulaLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Globulars"), AppCore, "ShowGlobularLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Open Clusters"), AppCore, "ShowOpenClusterLabels"),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Locations")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Locations"), AppCore, "ShowLocationLabels"),
                    new AppCoreFloatItem(LocalizationHelper.Localize("Minimum Labeled Feature Size"), AppCore, "MinimumFeatureSize", 0, 99),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Cities"), AppCore, "ShowCityLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Observatories"), AppCore, "ShowObservatoryLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Landing Sites"), AppCore, "ShowLandingSiteLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Montes (Mountains)"), AppCore, "ShowMonsLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Maria (Seas)"), AppCore, "ShowMareLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Craters"), AppCore, "ShowCraterLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Valles (Valleys)"), AppCore, "ShowVallisLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Terrae (Land masses)"), AppCore, "ShowTerraLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Volcanoes"), AppCore, "ShowEruptiveCenterLabels"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Other"), AppCore, "ShowOtherLabels")
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "renderer")
            {
                var items = new SettingsCommonItem[]
                {
                    new AppCoreIntItem(LocalizationHelper.Localize("Texture Resolution"), AppCore, "Resolution", new string[] { LocalizationHelper.Localize("Low"), LocalizationHelper.Localize("Medium"), LocalizationHelper.Localize("High") }),
                    new AppCoreIntItem(LocalizationHelper.Localize("Star Style"), AppCore, "StarStyle", new []
                    {
                        LocalizationHelper.Localize("Fuzzy Points"),
                        LocalizationHelper.Localize("Points"),
                        LocalizationHelper.Localize("Scaled Discs")
                    }),

                    new AppCoreBooleanItem(LocalizationHelper.Localize("Smooth Lines"), AppCore, "ShowSmoothLines"),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Tinted Illumination"), AppCore, "ShowTintedIllumination"),

                    new AppCoreBooleanItem(LocalizationHelper.Localize("Auto Mag"), AppCore, "ShowAutoMag"),
                    new AppCoreFloatItem(LocalizationHelper.Localize("Ambient Light"), AppCore, "AmbientLightLevel", 0, 0.99f, 0.01f),
                    new AppCoreFloatItem(LocalizationHelper.Localize("Faintest Stars"), AppCore, "FaintestVisible", 3, 12),
                    new AppCoreFloatItem(LocalizationHelper.Localize("Galaxy Brightness"), AppCore, "GalaxyBrightness", 0, 1, 0.01f),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Advanced")),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("HiDPI"), AppSettings, "UseFullDPI"),
                    new AppSettingsBooleanItem(LocalizationHelper.Localize("Anti-aliasing"), AppSettings, "EnableMSAA"),
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "region")
            {
                var items = new SettingsCommonItem[]
                {
                    new AppCoreIntItem(LocalizationHelper.Localize("Info Display"), AppCore, "HudDetail", new []
                    {
                        LocalizationHelper.Localize("None"),
                        LocalizationHelper.Localize("Terse"),
                        LocalizationHelper.Localize("Verbose")
                    }),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Time")),
                    new AppCoreIntItem(LocalizationHelper.Localize("Time Zone"), AppCore, "TimeZone", new []
                    {
                        LocalizationHelper.Localize("Local Time"),
                        LocalizationHelper.Localize("UTC"),
                    }),
                    new AppCoreIntItem(LocalizationHelper.Localize("Date Format"), AppCore, "DateFormat", new []
                    {
                        LocalizationHelper.Localize("Default"),
                        LocalizationHelper.Localize("YYYY MMM DD HH:MM:SS TZ"),
                        LocalizationHelper.Localize("UTC Offset")
                    }),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Region")),
                    new AppCoreIntItem(LocalizationHelper.Localize("Measure Units"), AppCore, "MeasurementSystem", new []
                    {
                        LocalizationHelper.Localize("Metric"),
                        LocalizationHelper.Localize("Imperial")
                    }),
                    new AppCoreIntItem(LocalizationHelper.Localize("Temperature Scale"), AppCore, "TemperatureScale", new []
                    {
                        LocalizationHelper.Localize("Kelvin"),
                        LocalizationHelper.Localize("Celsius"),
                        LocalizationHelper.Localize("Fahrenheit")
                    }),
                    new LanguageIntItem(LocalizationHelper.Localize("Language"), AppSettings, AvailableLanguages),
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
