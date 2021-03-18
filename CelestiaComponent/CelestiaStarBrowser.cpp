#include "pch.h"
#include "CelestiaStarBrowser.h"
#if __has_include("CelestiaStarBrowser.g.cpp")
#include "CelestiaStarBrowser.g.cpp"
#endif

using namespace std;

#define BROWSER_MAX_STAR_COUNT          100

namespace winrt::CelestiaComponent::implementation
{
	CelestiaStarBrowser::CelestiaStarBrowser(StarBrowser* sb) : CelestiaStarBrowserT<CelestiaStarBrowser>(), sb(sb)
	{
	}

	com_array<CelestiaComponent::CelestiaStar> CelestiaStarBrowser::Stars()
	{
        std::vector<const Star*>* stars = sb->listStars(BROWSER_MAX_STAR_COUNT);
        if (stars == nullptr)
            return com_array<CelestiaComponent::CelestiaStar>();

        std::vector<CelestiaComponent::CelestiaStar> starVec;
        for (int i = 0; i < stars->size(); i++)
        {
            Star* aStar = (Star*)(*stars)[i];
            starVec.push_back(make<CelestiaStar>(aStar));
        }

        delete stars;
        return com_array<CelestiaComponent::CelestiaStar>(starVec);
	}

	CelestiaStarBrowser::~CelestiaStarBrowser()
	{
		delete sb;
	}
}
