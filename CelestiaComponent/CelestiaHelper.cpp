//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include <celengine/astro.h>
#include "CelestiaHelper.h"
#include "CelestiaVector.h"
#if __has_include("CelestiaHelper.g.cpp")
#include "CelestiaHelper.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	double CelestiaHelper::JulianDayFromDateTime(Windows::Foundation::DateTime const& dateTime)
	{
		Windows::Globalization::Calendar c;
		c.ChangeClock(Windows::Globalization::ClockIdentifiers::TwentyFourHour());
		c.ChangeCalendarSystem(Windows::Globalization::CalendarIdentifiers::Gregorian());
		c.ChangeTimeZone(L"UTC");
		c.SetDateTime(dateTime);
		int era = c.Era();
		int year = c.Year();
		if (era < 1) year = 1 - year;
		astro::Date astroDate(year, c.Month(), c.Day());
		astroDate.hour = c.Hour();
		astroDate.minute = c.Minute();
		astroDate.seconds = c.Second();
		return astro::UTCtoTDB(astroDate);
	}

	Windows::Foundation::DateTime CelestiaHelper::DateTimeFromJulianDay(double julianDay)
	{
		Windows::Globalization::Calendar c;
		c.ChangeClock(Windows::Globalization::ClockIdentifiers::TwentyFourHour());
		c.ChangeCalendarSystem(Windows::Globalization::CalendarIdentifiers::Gregorian());
		c.ChangeTimeZone(L"UTC");
        astro::Date astroDate = astro::TDBtoUTC(julianDay);
		int year = astroDate.year;

		int era = 1;
		if (year < 1)
		{
			era = 0;
			year = 1 - year;
		}
		c.Era(era);
		c.Year(year);
		c.Month(astroDate.month);
		c.Day(astroDate.day);
		c.Hour(astroDate.hour);
		c.Minute(astroDate.minute);
		c.Second((int32_t)floor(astroDate.seconds));
		return c.GetDateTime();
	}

    CelestiaComponent::CelestiaVector CelestiaHelper::CelToJ2000Ecliptic(CelestiaComponent::CelestiaVector const& cel)
    {
        auto obj = get_self<CelestiaVector>(cel);
        auto v = Eigen::Vector3d(obj->X(), obj->Y(), obj->Z());
        return make<CelestiaVector>(v.x(), -v.z(), v.y());
    }

    CelestiaComponent::CelestiaVector CelestiaHelper::EclipticToEquatorial(CelestiaComponent::CelestiaVector const& ecliptic)
    {
        auto obj = get_self<CelestiaVector>(ecliptic);
        auto v = Eigen::Vector3d(obj->X(), obj->Y(), obj->Z());
        auto transformed = astro::eclipticToEquatorial(v);
        return make<CelestiaVector>(transformed.x(), transformed.y(), transformed.z());
    }

    CelestiaComponent::CelestiaVector CelestiaHelper::EquatorialToGalactic(CelestiaComponent::CelestiaVector const& equatorial)
    {
        auto obj = get_self<CelestiaVector>(equatorial);
        auto v = Eigen::Vector3d(obj->X(), obj->Y(), obj->Z());
        auto transformed = astro::equatorialToGalactic(v);
        return make<CelestiaVector>(transformed.x(), transformed.y(), transformed.z());
    }

    CelestiaComponent::CelestiaVector CelestiaHelper::RectToSpherical(CelestiaComponent::CelestiaVector const& rect)
    {
        auto obj = get_self<CelestiaVector>(rect);
        auto v = Eigen::Vector3d(obj->X(), obj->Y(), obj->Z());
        double r = v.norm();
        double theta = atan2(v.y(), v.x());
        if (theta < 0)
            theta = theta + 2 * PI;
        double phi = asin(v.z() / r);
        return make<CelestiaVector>(theta, phi, r);
    }

    double CelestiaHelper::AUToKilometers(double au)
    {
        return astro::AUtoKilometers(au);
    }
}
