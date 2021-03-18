#include "pch.h"
#include "CelestiaEclipse.h"
#if __has_include("CelestiaEclipse.g.cpp")
#include "CelestiaEclipse.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaEclipse::CelestiaEclipse(Eclipse* e) : CelestiaEclipseT<CelestiaEclipse>(), startTime(e->startTime), endTime(endTime), occulter(nullptr), receiver(nullptr)
	{
		occulter = make<CelestiaBody>(e->occulter);
		receiver = make<CelestiaBody>(e->receiver);
	}

	CelestiaComponent::CelestiaBody CelestiaEclipse::Occulter()
	{
		return occulter;
	}

	CelestiaComponent::CelestiaBody CelestiaEclipse::Receiver()
	{
		return receiver;
	}

	double CelestiaEclipse::StartTime()
	{
		return startTime;
	}

	double CelestiaEclipse::EndTime()
	{
		return endTime;
	}
}
