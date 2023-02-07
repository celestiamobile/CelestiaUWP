//
// InfoPage.xaml.cs
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
using System;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Navigation;

namespace CelestiaUWP
{
    public sealed partial class InfoPage : Page
    {
        public InfoPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var parameter = ((CelestiaAppCore, CelestiaSelection))e.Parameter;
            var AppCore = parameter.Item1;
            var Selection = parameter.Item2;

            NameLabel.Text = AppCore.Simulation.Universe.NameForSelection(Selection);
            DetailLabel.Text = GetOverview(Selection, AppCore);
            var url = CelestiaAppHelper.GetInfoURL(Selection);
            if (!string.IsNullOrEmpty(url))
            {
                LinkButton.NavigateUri = new Uri(url);
                LinkButton.Content = url;
            }
        }

        private string GetOverview(CelestiaSelection Selection, CelestiaAppCore AppCore)
        {
            var obj = Selection.Object;
            if (obj is CelestiaBody)
                return GetBodyOverview((CelestiaBody)obj, AppCore);
            if (obj is CelestiaStar)
                return GetStarOverview((CelestiaStar)obj, AppCore);
            if (obj is CelestiaDSO)
                return GetDSOOveriew((CelestiaDSO)obj);
            return LocalizationHelper.Localize("No overview available.");
        }

        private string GetBodyOverview(CelestiaBody Body, CelestiaAppCore AppCore)
        {
            var str = "";
            var radius = Body.Radius;
            string radiusString;
            const float oneMiInKm = 1.609344f;
            const float oneFtInKm = 0.0003048f;
            if (AppCore.MeasurementSystem == (int)CelestiaMeasurementSystem.imperial)
            {
                if (radius >= oneMiInKm)
                    radiusString = string.Format(LocalizationHelper.Localize("%d mi").Replace("%d", "{0}"), (int)(radius / oneMiInKm));
                else
                    radiusString = string.Format(LocalizationHelper.Localize("%d ft").Replace("%d", "{0}"), (int)(radius / oneFtInKm));
            }
            else
            {
                if (radius >= 1.0f)
                    radiusString = string.Format(LocalizationHelper.Localize("%d km").Replace("%d", "{0}"), (int)radius);
                else
                    radiusString = string.Format(LocalizationHelper.Localize("%d m").Replace("%d", "{0}"), (int)(radius * 1000.0f));
            }
            if (Body.IsEllipsoid)
                str += string.Format(LocalizationHelper.Localize("Equatorial radius: %s").Replace("%s", "{0}"), radiusString);
            else
                str += string.Format(LocalizationHelper.Localize("Size: %s").Replace("%s", "{0}"), radiusString);
            var time = AppCore.Simulation.Time;
            var orbit = Body.OrbitAtTime(time);
            var rotation = Body.RotationModelAtTime(time);
            var orbitalPeriod = orbit.IsPeriodic ? orbit.Period : 0.0;
            if (rotation.IsPeriodic && Body.Type != CelestiaBodyType.spacecraft)
            {
                var rotPeriod = rotation.Period;
                var dayLength = 0.0;
                if (orbit.IsPeriodic)
                {
                    var siderealDaysPerYear = orbitalPeriod / rotPeriod;
                    var solarDaysPerYear = siderealDaysPerYear - 1.0;
                    if (solarDaysPerYear > 0.0001)
                    {
                        dayLength = orbitalPeriod / (siderealDaysPerYear - 1.0);
                    }
                }
                string unitTemplate;
                if (rotPeriod < 2.0)
                {
                    rotPeriod *= 24;
                    dayLength *= 24;

                    unitTemplate = LocalizationHelper.Localize("%.2f hours").Replace("%.2f", "{0:N2}");
                }
                else
                {
                    unitTemplate = LocalizationHelper.Localize("%.2f days").Replace("%.2f", "{0:N2}");
                }
                str += "\n";
                str += string.Format(LocalizationHelper.Localize("Sidereal rotation period: %s").Replace("%s", "{0}"), string.Format(unitTemplate, rotPeriod));

                if (dayLength != 0.0)
                {
                    str += "\n";
                    str += string.Format(LocalizationHelper.Localize("Length of day: %s").Replace("%s", "{0}"), string.Format(unitTemplate, dayLength));
                }
                if (Body.HasRings)
                {
                    str += "\n";
                    str += LocalizationHelper.Localize("Has rings");
                }

                if (Body.HasAtmosphere)
                {
                    str += "\n";
                    str += LocalizationHelper.Localize("Has atmosphere");
                }
            }

            return str;
        }

        private string GetStarOverview(CelestiaStar Star, CelestiaAppCore AppCore)
        {
            var str = "";

            var time = AppCore.Simulation.Time;
            var celPos = Star.PositionAtTime(time).OffsetFrom(CelestiaUniversalCoord.Zero);
            var eqPos = CelestiaHelper.EclipticToEquatorial(CelestiaHelper.CelToJ2000Ecliptic(celPos));
            var sph = CelestiaHelper.RectToSpherical(eqPos);

            var hms = new CelestiaDMS(sph.X);
            str += string.Format(LocalizationHelper.Localize("RA: {0}h {1}m {2:N2}s"), hms.Hours, hms.Minutes, hms.Seconds);

            str += "\n";
            var dms = new CelestiaDMS(sph.Y);
            str += string.Format(LocalizationHelper.Localize("DEC: {0}° {1}′ {2:N2}″"), dms.Hours, dms.Minutes, dms.Seconds);

            return str;
        }

        private string GetDSOOveriew(CelestiaDSO DSO)
        {
            var str = "";

            var celPos = DSO.Position;
            var eqPos = CelestiaHelper.EclipticToEquatorial(CelestiaHelper.CelToJ2000Ecliptic(celPos));
            var sph = CelestiaHelper.RectToSpherical(eqPos);

            var hms = new CelestiaDMS(sph.X);
            str += string.Format(LocalizationHelper.Localize("RA: {0}h {1}m {2:N2}s"), hms.Hours, hms.Minutes, hms.Seconds);

            str += "\n";
            var dms = new CelestiaDMS(sph.Y);
            str += string.Format(LocalizationHelper.Localize("DEC: {0}° {1}′ {2:N2}″"), dms.Hours, dms.Minutes, dms.Seconds);

            var galPos = CelestiaHelper.EquatorialToGalactic(eqPos);
            sph = CelestiaHelper.RectToSpherical(galPos);

            str += "\n";
            dms = new CelestiaDMS(sph.X);
            str += string.Format(LocalizationHelper.Localize("L: {0}° {1}′ {2:N2}″"), dms.Hours, dms.Minutes, dms.Seconds);

            str += "\n";
            dms = new CelestiaDMS(sph.Y);
            str += string.Format(LocalizationHelper.Localize("B: {0}° {1}′ {2:N2}″"), dms.Hours, dms.Minutes, dms.Seconds);

            return str;
        }
    }
}
