#include "pch.h"
#include "LocalizationHelper.h"
#if __has_include("LocalizationHelper.g.cpp")
#include "LocalizationHelper.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaAppComponent::implementation
{
    static hstring currentLocale = L"en";

    hstring LocalizationHelper::Locale()
    {
        return currentLocale;
    }

    void LocalizationHelper::Locale(hstring const& locale)
    {
        currentLocale = locale;
    }

    hstring LocalizationHelper::Localize(hstring const& original)
    {
        return CelestiaComponent::CelestiaAppCore::LocalizedString(original, L"celestia_ui");
    }
}
