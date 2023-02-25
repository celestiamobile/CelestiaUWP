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

            if (item.Tag == "display")
            {
                var items = new SettingsCommonItem[]
                {
                    new SettingsHeaderItem(LocalizationHelper.Localize("Objects")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars"), AppCore, CelestiaSettingBooleanEntry.ShowStars),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets"), AppCore, CelestiaSettingBooleanEntry.ShowPlanets),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets"), AppCore, CelestiaSettingBooleanEntry.ShowDwarfPlanets),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMoons),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMinorMoons),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids"), AppCore, CelestiaSettingBooleanEntry.ShowAsteroids),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets"), AppCore, CelestiaSettingBooleanEntry.ShowComets),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft"), AppCore, CelestiaSettingBooleanEntry.ShowSpacecrafts),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galaxies"), AppCore, CelestiaSettingBooleanEntry.ShowGalaxies),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Nebulae"), AppCore, CelestiaSettingBooleanEntry.ShowNebulae),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Globulars"), AppCore, CelestiaSettingBooleanEntry.ShowGlobulars),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Open Clusters"), AppCore, CelestiaSettingBooleanEntry.ShowOpenClusters),
                    new SettingsHeaderItem(LocalizationHelper.Localize("Features")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Clouds"), AppCore, CelestiaSettingBooleanEntry.ShowCloudMaps),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Atmospheres"), AppCore, CelestiaSettingBooleanEntry.ShowAtmospheres),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comet Tails"), AppCore, CelestiaSettingBooleanEntry.ShowCometTails),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planet Rings"), AppCore, CelestiaSettingBooleanEntry.ShowPlanetRings),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ring Shadows"), AppCore, CelestiaSettingBooleanEntry.ShowRingShadows),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Eclipse Shadows"), AppCore, CelestiaSettingBooleanEntry.ShowEclipseShadows),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Cloud Shadows"), AppCore, CelestiaSettingBooleanEntry.ShowCloudShadows),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Night Lights"), AppCore, CelestiaSettingBooleanEntry.ShowNightMaps),
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "guides")
            {
                var items = new SettingsCommonItem[]
                {
                    new SettingsHeaderItem(LocalizationHelper.Localize("Orbits")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Orbits"), AppCore, CelestiaSettingBooleanEntry.ShowOrbits),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Fading Orbits"), AppCore, CelestiaSettingBooleanEntry.ShowFadingOrbits),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Partial Trajectories"), AppCore, CelestiaSettingBooleanEntry.ShowPartialTrajectories),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars"), AppCore, CelestiaSettingBooleanEntry.ShowStellarOrbits),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets"), AppCore, CelestiaSettingBooleanEntry.ShowPlanetOrbits),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets"), AppCore, CelestiaSettingBooleanEntry.ShowDwarfPlanetOrbits),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMoonOrbits),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMinorMoonOrbits),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids"), AppCore, CelestiaSettingBooleanEntry.ShowAsteroidOrbits),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets"), AppCore, CelestiaSettingBooleanEntry.ShowCometOrbits),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft"), AppCore, CelestiaSettingBooleanEntry.ShowSpacecraftOrbits),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Constellations")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Diagrams"), AppCore, CelestiaSettingBooleanEntry.ShowDiagrams),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Labels"), AppCore, CelestiaSettingBooleanEntry.ShowConstellationLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Labels in Latin"), AppCore, CelestiaSettingBooleanEntry.ShowLatinConstellationLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Boundaries"), AppCore, CelestiaSettingBooleanEntry.ShowBoundaries),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Grids")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Equatorial"), AppCore, CelestiaSettingBooleanEntry.ShowCelestialSphere),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ecliptic"), AppCore, CelestiaSettingBooleanEntry.ShowEclipticGrid),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Horizontal"), AppCore, CelestiaSettingBooleanEntry.ShowHorizonGrid),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galactic"), AppCore, CelestiaSettingBooleanEntry.ShowGalacticGrid),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Miscellaneous")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Markers"), AppCore, CelestiaSettingBooleanEntry.ShowMarkers),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Ecliptic Line"), AppCore, CelestiaSettingBooleanEntry.ShowEcliptic)
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "labels")
            {
                var items = new SettingsCommonItem[]
                {
                    new SettingsHeaderItem(LocalizationHelper.Localize("Objects")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Stars"), AppCore, CelestiaSettingBooleanEntry.ShowStarLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Planets"), AppCore, CelestiaSettingBooleanEntry.ShowPlanetLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Dwarf Planets"), AppCore, CelestiaSettingBooleanEntry.ShowDwarfPlanetLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMoonLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Minor Moons"), AppCore, CelestiaSettingBooleanEntry.ShowMinorMoonLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Asteroids"), AppCore, CelestiaSettingBooleanEntry.ShowAsteroidLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Comets"), AppCore, CelestiaSettingBooleanEntry.ShowCometLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Spacecraft"), AppCore, CelestiaSettingBooleanEntry.ShowSpacecraftLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Galaxies"), AppCore, CelestiaSettingBooleanEntry.ShowGalaxyLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Nebulae"), AppCore, CelestiaSettingBooleanEntry.ShowNebulaLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Globulars"), AppCore, CelestiaSettingBooleanEntry.ShowGlobularLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Open Clusters"), AppCore, CelestiaSettingBooleanEntry.ShowOpenClusterLabels),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Locations")),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Show Locations"), AppCore, CelestiaSettingBooleanEntry.ShowLocationLabels),
                    new AppCoreFloatItem(LocalizationHelper.Localize("Minimum Labeled Feature Size"), AppCore, CelestiaSettingSingleEntry.MinimumFeatureSize, 0, 99),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Cities"), AppCore, CelestiaSettingBooleanEntry.ShowCityLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Observatories"), AppCore, CelestiaSettingBooleanEntry.ShowObservatoryLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Landing Sites"), AppCore, CelestiaSettingBooleanEntry.ShowLandingSiteLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Montes (Mountains)"), AppCore, CelestiaSettingBooleanEntry.ShowMonsLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Maria (Seas)"), AppCore, CelestiaSettingBooleanEntry.ShowMareLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Craters"), AppCore, CelestiaSettingBooleanEntry.ShowCraterLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Valles (Valleys)"), AppCore, CelestiaSettingBooleanEntry.ShowVallisLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Terrae (Land masses)"), AppCore, CelestiaSettingBooleanEntry.ShowTerraLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Volcanoes"), AppCore, CelestiaSettingBooleanEntry.ShowEruptiveCenterLabels),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Other"), AppCore, CelestiaSettingBooleanEntry.ShowOtherLabels)
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "renderer")
            {
                var items = new SettingsCommonItem[]
                {
                    new AppCoreIntItem(LocalizationHelper.Localize("Texture Resolution"), AppCore, CelestiaSettingInt32Entry.Resolution, new string[] { LocalizationHelper.Localize("Low"), LocalizationHelper.Localize("Medium"), LocalizationHelper.Localize("High") }),
                    new AppCoreIntItem(LocalizationHelper.Localize("Star Style"), AppCore, CelestiaSettingInt32Entry.StarStyle, new []
                    {
                        LocalizationHelper.Localize("Fuzzy Points"),
                        LocalizationHelper.Localize("Points"),
                        LocalizationHelper.Localize("Scaled Discs")
                    }),

                    new AppCoreBooleanItem(LocalizationHelper.Localize("Smooth Lines"), AppCore, CelestiaSettingBooleanEntry.ShowSmoothLines),
                    new AppCoreBooleanItem(LocalizationHelper.Localize("Tinted Illumination"), AppCore, CelestiaSettingBooleanEntry.ShowTintedIllumination),

                    new AppCoreBooleanItem(LocalizationHelper.Localize("Auto Mag"), AppCore, CelestiaSettingBooleanEntry.ShowAutoMag),
                    new AppCoreFloatItem(LocalizationHelper.Localize("Ambient Light"), AppCore, CelestiaSettingSingleEntry.AmbientLightLevel, 0, 0.99f, 0.01f),
                    new AppCoreFloatItem(LocalizationHelper.Localize("Faintest Stars"), AppCore, CelestiaSettingSingleEntry.FaintestVisible, 3, 12),
                    new AppCoreFloatItem(LocalizationHelper.Localize("Galaxy Brightness"), AppCore, CelestiaSettingSingleEntry.GalaxyBrightness, 0, 1, 0.01f),

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
                    new AppCoreIntItem(LocalizationHelper.Localize("Info Display"), AppCore, CelestiaSettingInt32Entry.HudDetail, new []
                    {
                        LocalizationHelper.Localize("None"),
                        LocalizationHelper.Localize("Terse"),
                        LocalizationHelper.Localize("Verbose")
                    }),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Time")),
                    new AppCoreIntItem(LocalizationHelper.Localize("Time Zone"), AppCore, CelestiaSettingInt32Entry.TimeZone, new []
                    {
                        LocalizationHelper.Localize("Local Time"),
                        LocalizationHelper.Localize("UTC"),
                    }),
                    new AppCoreIntItem(LocalizationHelper.Localize("Date Format"), AppCore, CelestiaSettingInt32Entry.DateFormat, new []
                    {
                        LocalizationHelper.Localize("Default"),
                        LocalizationHelper.Localize("YYYY MMM DD HH:MM:SS TZ"),
                        LocalizationHelper.Localize("UTC Offset")
                    }),

                    new SettingsHeaderItem(LocalizationHelper.Localize("Region")),
                    new AppCoreIntItem(LocalizationHelper.Localize("Measure Units"), AppCore, CelestiaSettingInt32Entry.MeasurementSystem, new []
                    {
                        LocalizationHelper.Localize("Metric"),
                        LocalizationHelper.Localize("Imperial")
                    }),
                    new AppCoreIntItem(LocalizationHelper.Localize("Temperature Scale"), AppCore, CelestiaSettingInt32Entry.TemperatureScale, new []
                    {
                        LocalizationHelper.Localize("Kelvin"),
                        LocalizationHelper.Localize("Celsius"),
                        LocalizationHelper.Localize("Fahrenheit")
                    }),
                    new LanguageIntItem(LocalizationHelper.Localize("Language"), AppSettings, AvailableLanguages),
                };
                Container.Navigate(typeof(SettingsCommonPage), items);
            }
            else if (item.Tag == "advanced")
            {
                var items = new SettingsCommonItem[]
                {
                    new SettingsHeaderItem(LocalizationHelper.Localize("Security")),
                    new AppCoreIntItem(LocalizationHelper.Localize("Script System Access Policy"), AppCore, CelestiaSettingInt32Entry.ScriptSystemAccessPolicy, new []
                    {
                        LocalizationHelper.Localize("Ask"),
                        LocalizationHelper.Localize("Allow"),
                        LocalizationHelper.Localize("Deny")
                    }, LocalizationHelper.Localize("This policy decides whether Lua scripts have access to the files on the system or not.")),
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
