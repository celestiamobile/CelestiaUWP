//
// SelectionHelper.cpp
//
// Copyright (C) 2024-present, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "SelectionHelper.h"
#if __has_include("SelectionHelper.g.cpp")
#include "SelectionHelper.g.cpp"
#endif

#include <fmt/printf.h>
#include <fmt/xchar.h>

using namespace winrt;
using namespace CelestiaComponent;
using namespace Windows::Globalization::NumberFormatting;

namespace winrt::CelestiaAppComponent::implementation
{
    hstring JoinLines(std::vector<std::wstring> const& lines)
    {
        hstring result;
        for (auto const& line : lines)
        {
            if (!result.empty())
                result = result + L"\n";
            result = result + hstring(line);
        }
        return result;
    }

    hstring GetBodyOverview(CelestiaBody const& body, CelestiaAppCore const& appCore)
    {
        std::vector<std::wstring> lines;
        auto radius = body.Radius();
        std::wstring radiusString = L"";
        const float oneMiInKm = 1.609344f;
        const float oneFtInKm = 0.0003048f;
        DecimalFormatter numberFormatter;
        numberFormatter.FractionDigits(0);
        numberFormatter.IsGrouped(true);
        if (static_cast<CelestiaMeasurementSystem>(appCore.MeasurementSystem()) == CelestiaMeasurementSystem::Imperial)
        {
            if (radius > oneMiInKm)
                radiusString = fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"%s mi", L"Unit mile")), std::wstring(numberFormatter.FormatInt(static_cast<int64_t>(radius / oneMiInKm))));
            else
                radiusString = fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"%s ft", L"Unit foot")), std::wstring(numberFormatter.FormatInt(static_cast<int64_t>(radius / oneFtInKm))));
        }
        else
        {
            if (radius > 1.0f)
                radiusString = fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"%s km", L"Unit kilometer")), std::wstring(numberFormatter.FormatInt(static_cast<int64_t>(radius))));
            else
                radiusString = fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"%s m", L"Unit meter")), std::wstring(numberFormatter.FormatInt(static_cast<int64_t>(radius * 1000.0f))));
        }
        if (body.IsEllipsoid())
            lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Equatorial radius: %s", L"")), radiusString));
        else
            lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Size: %s", L"Size of an object")), radiusString));
        auto julianDay = appCore.Simulation().JulianDay();
        auto orbit = body.OrbitAtTime(julianDay);
        auto rotation = body.RotationModelAtTime(julianDay);
        auto orbitalPeriod = orbit.IsPeriodic() ? orbit.Period() : 0.0;
        if (rotation.IsPeriodic() && body.Type() != CelestiaBodyType::Spacecraft)
        {
            auto rotPeriod = rotation.Period();
            auto dayLength = 0.0;
            if (orbit.IsPeriodic())
            {
                auto siderealDaysPerYear = orbitalPeriod / rotPeriod;
                auto solarDaysPerYear = siderealDaysPerYear - 1.0;
                if (solarDaysPerYear > 0.0001)
                {
                    dayLength = orbitalPeriod / (siderealDaysPerYear - 1.0);
                }
            }
            hstring unitTemplate = L"";
            if (rotPeriod < 2.0)
            {
                rotPeriod *= 24;
                dayLength *= 24;

                unitTemplate = LocalizationHelper::Localize(L"%s hours", L"");
            }
            else
            {
                unitTemplate = LocalizationHelper::Localize(L"%s days", L"");
            }
            rotPeriod = std::round(rotPeriod * 100.0) / 100.0;
            dayLength = std::round(dayLength * 100.0) / 100.0;
            lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Sidereal rotation period: %s", L"")), fmt::sprintf(std::wstring(unitTemplate), std::wstring(numberFormatter.FormatDouble(rotPeriod)))));

            if (dayLength != 0.0)
            {
                lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Length of day: %s", L"")), fmt::sprintf(std::wstring(unitTemplate), std::wstring(numberFormatter.FormatDouble(dayLength)))));
            }
        }
        if (body.HasRings())
        {
            lines.push_back(std::wstring(LocalizationHelper::Localize(L"Has rings", L"Indicate that an object has rings")));
        }

        if (body.HasAtmosphere())
        {
            lines.push_back(std::wstring(LocalizationHelper::Localize(L"Has atmosphere", L"Indicate that an object has atmosphere")));
        }

        auto timeline{ body.Timeline() };
        if (timeline.PhaseCount() > 0)
        {
            using namespace Windows::Globalization::DateTimeFormatting;
            DateTimeFormatter dateFormatter{ L"shortdate shorttime" };

            auto startJulianDay = timeline.PhaseAtIndex(0).StartJulianDay();
            auto endJulianDay = timeline.PhaseAtIndex(0).EndJulianDay();
            if (!std::isinf(startJulianDay))
            {
                if (startJulianDay < CelestiaHelper::MinRepresentableJulianDay() || startJulianDay > CelestiaHelper::MaxRepresentableJulianDay())
                {
                    startJulianDay = std::round(startJulianDay * 10000.0) / 10000.0;
                    lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Start Julian day: %s", L"Template for displaying when start time cannot be correctly formatted by the system")), std::wstring(numberFormatter.FormatDouble(startJulianDay))));
                }
                else
                {
                    auto startTime = CelestiaHelper::DateTimeFromJulianDay(startJulianDay);
                    lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Start time: %s", L"Template for the start time of a body, usually a spacecraft")), std::wstring(dateFormatter.Format(startTime))));
                }
            }
            if (!std::isinf(endJulianDay))
            {
                if (endJulianDay < CelestiaHelper::MinRepresentableJulianDay() || endJulianDay > CelestiaHelper::MaxRepresentableJulianDay())
                {
                    endJulianDay = std::round(endJulianDay * 10000.0) / 10000.0;
                    lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"End Julian day: %s", L"Template for displaying when end time cannot be correctly formatted by the system")), std::wstring(numberFormatter.FormatDouble(endJulianDay))));
                }
                else
                {
                    auto endTime = CelestiaHelper::DateTimeFromJulianDay(endJulianDay);
                    lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"End time: %s", L"Template for the end time of a body, usually a spacecraft")), std::wstring(dateFormatter.Format(endTime))));
                }
            }
        }

        return JoinLines(lines);
    }

    hstring GetStarOverview(CelestiaStar const& star, CelestiaAppCore const& appCore)
    {
        std::vector<std::wstring> lines;

        lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"Spectral type: %s", L"")), std::wstring(star.SpectralType())));

        auto julianDay = appCore.Simulation().JulianDay();
        auto celPos = star.PositionAtTime(julianDay).OffsetFrom(CelestiaUniversalCoord::Zero());
        auto eqPos = CelestiaHelper::EclipticToEquatorial(CelestiaHelper::CelToJ2000Ecliptic(celPos));
        auto sph = CelestiaHelper::RectToSpherical(eqPos);

        DecimalFormatter numberFormatter;
        numberFormatter.FractionDigits(0);
        numberFormatter.IsGrouped(true);

        CelestiaDMS hms{ CelestiaHelper::DegFromRad(sph.X()) };
        double seconds = std::round(hms.HMSSeconds() * 100.0) / 100.0;
        lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"RA: %sh %sm %ss", L"Equatorial coordinate")), std::wstring(numberFormatter.FormatInt(hms.HMSHours())), std::wstring(numberFormatter.FormatInt(hms.HMSMinutes())), std::wstring(numberFormatter.FormatDouble(seconds))));

        CelestiaDMS dms{ CelestiaHelper::DegFromRad(sph.Y()) };
        seconds = std::round(dms.Seconds() * 100.0) / 100.0;
        lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"DEC: %s\u00b0 %s\u2032 %s\u2033", L"Equatorial coordinate")), std::wstring(numberFormatter.FormatInt(dms.Degrees())), std::wstring(numberFormatter.FormatInt(dms.Minutes())), std::wstring(numberFormatter.FormatDouble(seconds))));

        return JoinLines(lines);
    }

    hstring GetDSOOveriew(CelestiaDSO const& dso, CelestiaAppCore const&)
    {
        std::vector<std::wstring> lines;

        if (!dso.Description().empty())
            lines.push_back(std::wstring(dso.Description()));

        auto celPos = dso.Position();
        auto eqPos = CelestiaHelper::EclipticToEquatorial(CelestiaHelper::CelToJ2000Ecliptic(celPos));
        auto sph = CelestiaHelper::RectToSpherical(eqPos);

        DecimalFormatter numberFormatter;
        numberFormatter.FractionDigits(0);
        numberFormatter.IsGrouped(true);

        CelestiaDMS hms{ CelestiaHelper::DegFromRad(sph.X()) };
        double seconds = std::round(hms.HMSSeconds() * 100.0) / 100.0;
        lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"RA: %sh %sm %ss", L"Equatorial coordinate")), std::wstring(numberFormatter.FormatInt(hms.HMSHours())), std::wstring(numberFormatter.FormatInt(hms.HMSMinutes())), std::wstring(numberFormatter.FormatDouble(seconds))));

        CelestiaDMS dms{ CelestiaHelper::DegFromRad(sph.Y()) };
        seconds = std::round(dms.Seconds() * 100.0) / 100.0;
        lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"DEC: %s\u00b0 %s\u2032 %s\u2033", L"Equatorial coordinate")), std::wstring(numberFormatter.FormatInt(dms.Degrees())), std::wstring(numberFormatter.FormatInt(dms.Minutes())), std::wstring(numberFormatter.FormatDouble(seconds))));

        auto galPos = CelestiaHelper::EquatorialToGalactic(eqPos);
        sph = CelestiaHelper::RectToSpherical(galPos);

        dms = CelestiaDMS(CelestiaHelper::DegFromRad(sph.X()));
        seconds = std::round(dms.Seconds() * 100.0) / 100.0;
        lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"L: %s\u00b0 %s\u2032 %s\u2033", L"Galactic coordinates")), std::wstring(numberFormatter.FormatInt(dms.Degrees())), std::wstring(numberFormatter.FormatInt(dms.Minutes())), std::wstring(numberFormatter.FormatDouble(seconds))));

        dms = CelestiaDMS(CelestiaHelper::DegFromRad(sph.Y()));
        seconds = std::round(dms.Seconds() * 100.0) / 100.0;
        lines.push_back(fmt::sprintf(std::wstring(LocalizationHelper::Localize(L"B: %s\u00b0 %s\u2032 %s\u2033", L"Galactic coordinates")), std::wstring(numberFormatter.FormatInt(dms.Degrees())), std::wstring(numberFormatter.FormatInt(dms.Minutes())), std::wstring(numberFormatter.FormatDouble(seconds))));

        return JoinLines(lines);
    }

    hstring SelectionHelper::GetOverview(CelestiaSelection const& selection, CelestiaAppCore const& appCore)
    {
        auto obj = selection.Object();
        if (obj == nullptr) return L"";
        auto body = obj.try_as<CelestiaBody>();
        if (body != nullptr)
        {
            return GetBodyOverview(body, appCore);
        }
        auto star = obj.try_as<CelestiaStar>();
        if (star != nullptr)
        {
            return GetStarOverview(star, appCore);
        }
        auto dso = obj.try_as<CelestiaDSO>();
        if (dso != nullptr)
        {
            return GetDSOOveriew(dso, appCore);
        }
        return LocalizationHelper::Localize(L"No overview available.", L"No overview for an object");
    }
}
