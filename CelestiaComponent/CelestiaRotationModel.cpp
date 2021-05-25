//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include "CelestiaHelper.h"
#include "CelestiaRotationModel.h"
#include "CelestiaVector.h"
#if __has_include("CelestiaRotationModel.g.cpp")
#include "CelestiaRotationModel.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    CelestiaRotationModel::CelestiaRotationModel(RotationModel* r) : CelestiaRotationModelT<CelestiaRotationModel>(), r(r)
    {
    }

    bool CelestiaRotationModel::IsPeriodic()
    {
        return r->isPeriodic();
    }

    double CelestiaRotationModel::Period()
    {
        return r->getPeriod();
    }

    double CelestiaRotationModel::ValidBeginTime()
    {
        double begin, end;
        r->getValidRange(begin, end);
        return begin;
    }

    double CelestiaRotationModel::ValidEndTime()
    {
        double begin, end;
        r->getValidRange(begin, end);
        return end;
    }

    CelestiaComponent::CelestiaVector CelestiaRotationModel::AngularVelocityAtTime(Windows::Foundation::DateTime const& time)
    {
        const Eigen::Vector3d v = r->angularVelocityAtTime(CelestiaHelper::JulianDayFromDateTime(time));
        return make<CelestiaVector>(v.x(), v.y(), v.z());
    }

    CelestiaComponent::CelestiaVector CelestiaRotationModel::EquatorOrientationAtTime(Windows::Foundation::DateTime const& time)
    {
        const Eigen::Quaterniond v = r->equatorOrientationAtTime(CelestiaHelper::JulianDayFromDateTime(time));
        return make<CelestiaVector>(v.x(), v.y(), v.z(), v.w());
    }

    CelestiaComponent::CelestiaVector CelestiaRotationModel::SpinAtTime(Windows::Foundation::DateTime const& time)
    {
        const Eigen::Quaterniond v = r->spin(CelestiaHelper::JulianDayFromDateTime(time));
        return make<CelestiaVector>(v.x(), v.y(), v.z(), v.w());
    }
}
