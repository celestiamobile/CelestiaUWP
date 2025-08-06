// CelestiaHelper.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include <celastro/astro.h>
#include <celastro/date.h>
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
        static auto epoch = celestia::astro::Date(1970, 1, 1);
        return celestia::astro::UTCtoTDB(epoch + std::chrono::duration_cast<std::chrono::milliseconds>(winrt::clock::to_sys(dateTime).time_since_epoch()).count() / 86400.0 / 1000.0);
	}

	Windows::Foundation::DateTime CelestiaHelper::DateTimeFromJulianDay(double julianDay)
	{
        static auto epoch = celestia::astro::Date(1970, 1, 1);
        auto date = (celestia::astro::TDBtoUTC(julianDay) - epoch) * 86400.0 * 1000.0;
        std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp{ std::chrono::milliseconds{static_cast<long long>(date)}};
        return winrt::clock::from_sys(tp);
	}

    double CelestiaHelper::MinRepresentableJulianDay()
    {
        static bool calculated = false;
        static double jd = 0.0;
        if (calculated)
            return jd;

        Windows::Globalization::Calendar c;
        c.ChangeClock(Windows::Globalization::ClockIdentifiers::TwentyFourHour());
        c.ChangeCalendarSystem(Windows::Globalization::CalendarIdentifiers::Gregorian());
        c.ChangeTimeZone(L"UTC");
        c.Era(1);
        c.Year(1);
        c.Month(1);
        c.Day(1);
        c.Hour(12); // With this all timezone's min JD will be >= 1/1/1
        c.Minute(0);
        c.Second(0);
        c.Nanosecond(0);
        jd = JulianDayFromDateTime(c.GetDateTime());
        calculated = true;
        return jd;
    }

    double CelestiaHelper::MaxRepresentableJulianDay()
    {
        static bool calculated = false;
        static double jd = 0.0;
        if (calculated)
            return jd;

        Windows::Globalization::Calendar c;
        c.ChangeClock(Windows::Globalization::ClockIdentifiers::TwentyFourHour());
        c.ChangeCalendarSystem(Windows::Globalization::CalendarIdentifiers::Gregorian());
        c.ChangeTimeZone(L"UTC");
        c.Era(1);
        c.Year(9999);
        c.Month(12);
        c.Day(31);
        c.Hour(11); // With this all timezone's max JD will be <= 9999/12/31
        c.Minute(59);
        c.Second(59);
        c.Nanosecond(0); // Avoid rounding and exceeds 11:59:59
        jd = JulianDayFromDateTime(c.GetDateTime());
        calculated = true;
        return jd;
    }

    Windows::Foundation::DateTime CelestiaHelper::GetMidnight(Windows::Foundation::DateTime const& dateTime)
    {
        Windows::Globalization::Calendar c;
        c.ChangeClock(Windows::Globalization::ClockIdentifiers::TwentyFourHour());
        c.ChangeCalendarSystem(Windows::Globalization::CalendarIdentifiers::Gregorian());
        c.SetDateTime(dateTime);
        c.Hour(0);
        c.Minute(0);
        c.Second(0);
        c.Nanosecond(0);
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
        auto transformed = celestia::astro::eclipticToEquatorial(v);
        return make<CelestiaVector>(transformed.x(), transformed.y(), transformed.z());
    }

    CelestiaComponent::CelestiaVector CelestiaHelper::EquatorialToGalactic(CelestiaComponent::CelestiaVector const& equatorial)
    {
        auto obj = get_self<CelestiaVector>(equatorial);
        auto v = Eigen::Vector3d(obj->X(), obj->Y(), obj->Z());
        auto transformed = celestia::astro::equatorialToGalactic(v);
        return make<CelestiaVector>(transformed.x(), transformed.y(), transformed.z());
    }

    CelestiaComponent::CelestiaVector CelestiaHelper::RectToSpherical(CelestiaComponent::CelestiaVector const& rect)
    {
        auto obj = get_self<CelestiaVector>(rect);
        auto v = Eigen::Vector3d(obj->X(), obj->Y(), obj->Z());
        double r = v.norm();
        double theta = atan2(v.y(), v.x());
        if (theta < 0)
            theta = theta + 2 * celestia::numbers::pi;
        double phi = asin(v.z() / r);
        return make<CelestiaVector>(theta, phi, r);
    }

    double CelestiaHelper::AUToKilometers(double au)
    {
        return celestia::astro::AUtoKilometers(au);
    }

    double CelestiaHelper::DegFromRad(double rad)
    {
        return celestia::math::radToDeg(rad);
    }
}
