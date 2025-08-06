// CelestiaEclipseFinder.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

#include "pch.h"
#include "CelestiaHelper.h"
#include "CelestiaEclipseFinder.h"
#if __has_include("CelestiaEclipseFinder.g.cpp")
#include "CelestiaEclipseFinder.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    class EclipseSeacherWatcher : public EclipseFinderWatcher
    {
    public:
        EclipseSeacherWatcher(CelestiaComponent::CelestiaEclipseFinder const& f) : EclipseFinderWatcher(), f(f) {}

        virtual Status eclipseFinderProgressUpdate(double t)
        {
            if (get_self<CelestiaEclipseFinder>(f)->aborted)
                return AbortOperation;
            return ContinueOperation;
        };
    private:
        CelestiaComponent::CelestiaEclipseFinder f;
    };

	CelestiaEclipseFinder::CelestiaEclipseFinder(CelestiaComponent::CelestiaBody body) : CelestiaEclipseFinderT<CelestiaEclipseFinder>(), aborted(true)
	{
        w = new EclipseSeacherWatcher(*this);
        f = new EclipseFinder(reinterpret_cast<Body*>(get_self<CelestiaBody>(body)->obj), w);
	}

	com_array<CelestiaComponent::CelestiaEclipse> CelestiaEclipseFinder::Search(CelestiaComponent::CelestiaEclipseKind kind, Windows::Foundation::DateTime const& startTime, Windows::Foundation::DateTime const& endTime)
	{
        aborted = false;
        std::vector<Eclipse> results;
        f->findEclipses(CelestiaHelper::JulianDayFromDateTime(startTime), CelestiaHelper::JulianDayFromDateTime(endTime), (int)kind, results);

        std::vector<CelestiaComponent::CelestiaEclipse> vec;
        for (Eclipse& result : results)
        {
            vec.push_back(make<CelestiaEclipse>(&result));
        }
        aborted = true;
		return com_array<CelestiaComponent::CelestiaEclipse>(vec);
	}

    void CelestiaEclipseFinder::Close()
    {
        if (w != nullptr)
        {
            delete w;
            w = nullptr;
        }
        if (f != nullptr)
        {
            delete f;
            f = nullptr;
        }
    }

    CelestiaEclipseFinder::~CelestiaEclipseFinder()
    {
        Close();
    }

    void CelestiaEclipseFinder::Abort()
    {
        aborted = true;
    }
}
