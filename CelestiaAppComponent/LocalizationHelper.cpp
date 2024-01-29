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

    hstring LocalizationHelper::Localize(hstring const& original, hstring const&)
    {
        return CelestiaComponent::CelestiaAppCore::LocalizedString(original, L"celestia_ui");
    }

    hstring LocalizationHelper::Localize(hstring const& original, hstring const& context, hstring const&)
    {
        return CelestiaComponent::CelestiaAppCore::LocalizedString(original, context, L"celestia_ui");
    }

    hstring LocalizationHelper::FromWindowsTag(hstring const& tag)
    {
        std::wstring langW = std::wstring(tag);
        std::wstring toReplace = L"-";
        auto loc = langW.find_first_of(toReplace);
        if (loc != std::wstring::npos)
        {
            langW.replace(loc, toReplace.size(), L"_");
            return hstring(langW);
        }
        return tag;
    }

    hstring LocalizationHelper::ToWindowsTag(hstring const& locale)
    {
        std::wstring langW = std::wstring(locale);
        std::wstring toReplace = L"_";
        auto loc = langW.find_first_of(toReplace);
        if (loc != std::wstring::npos)
        {
            langW.replace(loc, toReplace.size(), L"-");
            return hstring(langW);
        }
        return locale;
    }
}
