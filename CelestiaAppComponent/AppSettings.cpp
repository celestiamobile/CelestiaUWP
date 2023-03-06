#include "pch.h"
#include "AppSettings.h"
#if __has_include("AppSettings.g.cpp")
#include "AppSettings.g.cpp"
#endif

using namespace std;
using namespace winrt;
using namespace CelestiaComponent;

namespace winrt::CelestiaAppComponent::implementation
{
    AppSettings::AppSettings(Windows::Storage::ApplicationDataContainer const& settings)
    {
        const auto& propertySet{ settings.Values() };
        auto fullDPIResult = propertySet.TryLookup(L"FullDPI");
        if (fullDPIResult)
            useFullDPI = unbox_value_or(fullDPIResult, useFullDPI);
        auto enableMSAAResult = propertySet.TryLookup(L"EnableMSAA");
        if (enableMSAAResult)
            enableMSAA = unbox_value_or(enableMSAAResult, enableMSAA);
        auto languageOverrideResult = propertySet.TryLookup(L"LanguageOverride");
        if (languageOverrideResult)
            languageOverride = unbox_value_or(languageOverrideResult, languageOverride);
        auto lastNewsIDResult = propertySet.TryLookup(L"LastNewsID");
        if (enableMSAAResult)
            lastNewsID = unbox_value_or(lastNewsIDResult, lastNewsID);
        auto gamepadRemapAResult = propertySet.TryLookup(L"GamepadRemapA");
        if (gamepadRemapAResult)
            gamepadRemapA = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapAResult, static_cast<int>(gamepadRemapA)));
        auto gamepadRemapBResult = propertySet.TryLookup(L"GamepadRemapB");
        if (gamepadRemapBResult)
            gamepadRemapB = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapBResult, static_cast<int>(gamepadRemapB)));
        auto gamepadRemapXResult = propertySet.TryLookup(L"GamepadRemapX");
        if (gamepadRemapXResult)
            gamepadRemapX = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapXResult, static_cast<int>(gamepadRemapX)));
        auto gamepadRemapYResult = propertySet.TryLookup(L"GamepadRemapY");
        if (gamepadRemapYResult)
            gamepadRemapY = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapYResult, static_cast<int>(gamepadRemapY)));
        auto gamepadRemapLTResult = propertySet.TryLookup(L"GamepadRemapLT");
        if (gamepadRemapLTResult)
            gamepadRemapLT = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapLTResult, static_cast<int>(gamepadRemapLT)));
        auto gamepadRemapRTResult = propertySet.TryLookup(L"GamepadRemapRT");
        if (gamepadRemapRTResult)
            gamepadRemapRT = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapRTResult, static_cast<int>(gamepadRemapRT)));
        auto gamepadRemapLBResult = propertySet.TryLookup(L"GamepadRemapLB");
        if (gamepadRemapLBResult)
            gamepadRemapLB = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapLBResult, static_cast<int>(gamepadRemapLB)));
        auto gamepadRemapRBResult = propertySet.TryLookup(L"GamepadRemapRB");
        if (gamepadRemapRBResult)
            gamepadRemapRB = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapRBResult, static_cast<int>(gamepadRemapRB)));
        auto gamepadRemapDpadLeftResult = propertySet.TryLookup(L"GamepadRemapDpadLeft");
        if (gamepadRemapDpadLeftResult)
            gamepadRemapDpadLeft = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapDpadLeftResult, static_cast<int>(gamepadRemapDpadLeft)));
        auto gamepadRemapDpadRightResult = propertySet.TryLookup(L"GamepadRemapDpadRight");
        if (gamepadRemapDpadRightResult)
            gamepadRemapDpadRight = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapDpadRightResult, static_cast<int>(gamepadRemapDpadRight)));
        auto gamepadRemapDpadUpResult = propertySet.TryLookup(L"GamepadRemapDpadUp");
        if (gamepadRemapDpadUpResult)
            gamepadRemapDpadUp = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapDpadUpResult, static_cast<int>(gamepadRemapDpadUp)));
        auto gamepadRemapDpadDownResult = propertySet.TryLookup(L"GamepadRemapDpadDown");
        if (gamepadRemapDpadDownResult)
            gamepadRemapDpadDown = static_cast<CelestiaGamepadAction>(unbox_value_or(gamepadRemapDpadDownResult, static_cast<int>(gamepadRemapDpadDown)));
        auto gamepadInvertXResult = propertySet.TryLookup(L"GamepadInvertX");
        if (gamepadInvertXResult)
            gamepadInvertX = unbox_value_or(gamepadInvertXResult, gamepadInvertX);
        auto gamepadInvertYResult = propertySet.TryLookup(L"GamepadInvertY");
        if (gamepadInvertYResult)
            gamepadInvertY = unbox_value_or(gamepadInvertYResult, gamepadInvertY);
    }

    bool AppSettings::UseFullDPI()
    {
        return useFullDPI;
    }

    void AppSettings::UseFullDPI(bool value)
    {
        useFullDPI = value;
    }

    bool AppSettings::EnableMSAA()
    {
        return enableMSAA;
    }

    void AppSettings::EnableMSAA(bool value)
    {
        enableMSAA = value;
    }

    hstring AppSettings::LanguageOverride()
    {
        return languageOverride;
    }

    void AppSettings::LanguageOverride(hstring const& value)
    {
        languageOverride = value;
    }

    hstring AppSettings::LastNewsID()
    {
        return lastNewsID;
    }

    void AppSettings::LastNewsID(hstring const& value)
    {
        lastNewsID = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapA()
    {
        return gamepadRemapA;
    }

    void AppSettings::GamepadRemapA(CelestiaGamepadAction value)
    {
        gamepadRemapA = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapB()
    {
        return gamepadRemapB;
    }

    void AppSettings::GamepadRemapB(CelestiaGamepadAction value)
    {
        gamepadRemapB = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapX()
    {
        return gamepadRemapX;
    }

    void AppSettings::GamepadRemapX(CelestiaGamepadAction value)
    {
        gamepadRemapX = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapY()
    {
        return gamepadRemapY;
    }

    void AppSettings::GamepadRemapY(CelestiaGamepadAction value)
    {
        gamepadRemapY = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapLT()
    {
        return gamepadRemapLT;
    }

    void AppSettings::GamepadRemapLT(CelestiaGamepadAction value)
    {
        gamepadRemapLT = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapRT()
    {
        return gamepadRemapRT;
    }

    void AppSettings::GamepadRemapRT(CelestiaGamepadAction value)
    {
        gamepadRemapRT = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapLB()
    {
        return gamepadRemapLB;
    }

    void AppSettings::GamepadRemapLB(CelestiaGamepadAction value)
    {
        gamepadRemapLB = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapRB()
    {
        return gamepadRemapRB;
    }

    void AppSettings::GamepadRemapRB(CelestiaGamepadAction value)
    {
        gamepadRemapRB = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapDpadUp()
    {
        return gamepadRemapDpadUp;
    }

    void AppSettings::GamepadRemapDpadUp(CelestiaGamepadAction value)
    {
        gamepadRemapDpadUp = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapDpadDown()
    {
        return gamepadRemapDpadDown;
    }

    void AppSettings::GamepadRemapDpadDown(CelestiaGamepadAction value)
    {
        gamepadRemapDpadDown = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapDpadLeft()
    {
        return gamepadRemapDpadLeft;
    }

    void AppSettings::GamepadRemapDpadLeft(CelestiaGamepadAction value)
    {
        gamepadRemapDpadLeft = value;
    }

    CelestiaGamepadAction AppSettings::GamepadRemapDpadRight()
    {
        return gamepadRemapDpadRight;
    }

    void AppSettings::GamepadRemapDpadRight(CelestiaGamepadAction value)
    {
        gamepadRemapDpadRight = value;
    }

    bool AppSettings::GamepadInvertX()
    {
        return gamepadInvertX;
    }

    void AppSettings::GamepadInvertX(bool value)
    {
        gamepadInvertX = value;
    }

    bool AppSettings::GamepadInvertY()
    {
        return gamepadInvertY;
    }

    void AppSettings::GamepadInvertY(bool value)
    {
        gamepadInvertY = value;
    }

    void AppSettings::Save(Windows::Storage::ApplicationDataContainer const& settings)
    {
        settings.Values().Insert(L"FullDPI", box_value(useFullDPI));
        settings.Values().Insert(L"EnableMSAA", box_value(enableMSAA));
        settings.Values().Insert(L"LanguageOverride", box_value(languageOverride));
        settings.Values().Insert(L"LastNewsID", box_value(lastNewsID));
        settings.Values().Insert(L"GamepadRemapA", box_value(static_cast<int>(gamepadRemapA)));
        settings.Values().Insert(L"GamepadRemapB", box_value(static_cast<int>(gamepadRemapB)));
        settings.Values().Insert(L"GamepadRemapX", box_value(static_cast<int>(gamepadRemapX)));
        settings.Values().Insert(L"GamepadRemapY", box_value(static_cast<int>(gamepadRemapY)));
        settings.Values().Insert(L"GamepadRemapLT", box_value(static_cast<int>(gamepadRemapLT)));
        settings.Values().Insert(L"GamepadRemapRT", box_value(static_cast<int>(gamepadRemapRT)));
        settings.Values().Insert(L"GamepadRemapLB", box_value(static_cast<int>(gamepadRemapLB)));
        settings.Values().Insert(L"GamepadRemapRB", box_value(static_cast<int>(gamepadRemapRB)));
        settings.Values().Insert(L"GamepadRemapDpadUp", box_value(static_cast<int>(gamepadRemapDpadUp)));
        settings.Values().Insert(L"GamepadRemapDpadDown", box_value(static_cast<int>(gamepadRemapDpadDown)));
        settings.Values().Insert(L"GamepadRemapDpadLeft", box_value(static_cast<int>(gamepadRemapDpadLeft)));
        settings.Values().Insert(L"GamepadRemapDpadRight", box_value(static_cast<int>(gamepadRemapDpadRight)));
        settings.Values().Insert(L"GamepadInvertX", box_value(gamepadInvertX));
        settings.Values().Insert(L"GamepadInvertY", box_value(gamepadInvertY));
    }
}
