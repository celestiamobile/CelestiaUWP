#include "pch.h"
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

	com_array<CelestiaComponent::CelestiaEclipse> CelestiaEclipseFinder::Search(CelestiaComponent::CelestiaEclipseKind kind, double startTime, double endTime)
	{
        aborted = false;
        std::vector<Eclipse> results;
        f->findEclipses(startTime, endTime, (int)kind, results);

        std::vector<CelestiaComponent::CelestiaEclipse> vec;
        for (Eclipse& result : results)
        {
            vec.push_back(make<CelestiaEclipse>(&result));
        }
        aborted = true;
		return com_array<CelestiaComponent::CelestiaEclipse>(vec);
	}

    CelestiaEclipseFinder::~CelestiaEclipseFinder()
    {
        delete f;
        delete w;
    }
}
