//
// Copyright © 2021 Celestia Development Team. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//

#include "pch.h"
#include <celestia/celestiastate.h>
#include <celestia/configfile.h>
#include <celestia/helper.h>
#include <celestia/progressnotifier.h>
#include <celestia/url.h>
#include <celutil/flag.h>
#ifdef ENABLE_NLS
#include <celutil/gettext.h>
#include <fmt/format.h>
#endif
#include <icu.h>
#include "CelestiaAppCore.h"
#include "CelestiaSelection.h"
#if __has_include("CelestiaAppCore.g.cpp")
#include "CelestiaAppCore.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
    class AppCoreProgressWatcher : public ProgressNotifier
    {
    public:
        AppCoreProgressWatcher(CelestiaComponent::CelestiaLoadCallback const& loadCallback) : ProgressNotifier(), loadCallback(loadCallback) {};
        void update(const string& status)
        {
            loadCallback(to_hstring(status));
        }
    private:
        CelestiaComponent::CelestiaLoadCallback loadCallback;
    };

    class AppCoreContextMenuHandler : public CelestiaCore::ContextMenuHandler
    {
    public:
        AppCoreContextMenuHandler(weak_ref<CelestiaAppCore> const& weakAppCore) : CelestiaCore::ContextMenuHandler(), weakAppCore(weakAppCore) {};

        void requestContextMenu(float x, float y, Selection sel)
        {
            auto appCore = weakAppCore.get();
            if (appCore == nullptr) return;

            appCore->showContextMenuEvent(*appCore, make<ShowContextMenuArgs>(x, y, make<CelestiaSelection>(sel)));
        }
    private:
        weak_ref<CelestiaAppCore> weakAppCore;
    };

    class AppCoreAlerter : public CelestiaCore::Alerter
    {
    public:
        AppCoreAlerter(weak_ref<CelestiaAppCore> const& weakAppCore) : CelestiaCore::Alerter(), weakAppCore(weakAppCore) {};

        void fatalError(const std::string& message)
        {
            auto appCore = weakAppCore.get();
            if (appCore == nullptr) return;

            appCore->fatalErrorEvent(*appCore, make<FatalErrorArgs>(to_hstring(message)));
        }
    private:
        weak_ref<CelestiaAppCore> weakAppCore;
    };

    class AppCoreCursorHandler : public CelestiaCore::CursorHandler
    {
    public:
        AppCoreCursorHandler(weak_ref<CelestiaAppCore> const& weakAppCore) : CelestiaCore::CursorHandler(), weakAppCore(weakAppCore) {};

        void setCursorShape(CelestiaCore::CursorShape cursorShape)
        {
            auto appCore = weakAppCore.get();
            if (appCore == nullptr) return;

            appCore->changeCursorEvent(*appCore, make<ChangeCursorArgs>(static_cast<CelestiaComponent::Cursor>(cursorShape)));
        }

        CelestiaCore::CursorShape getCursorShape() const { return CelestiaCore::ArrowCursor; }
    private:
        weak_ref<CelestiaAppCore> weakAppCore;
    };

    CelestiaAppCore::CelestiaAppCore() : CelestiaAppCoreT<CelestiaAppCore>(), sim(nullptr)
    {
        core = new CelestiaCore;
        core->setContextMenuHandler(new AppCoreContextMenuHandler(get_weak()));
        core->setAlerter(new AppCoreAlerter(get_weak()));
        core->setCursorHandler(new AppCoreCursorHandler(get_weak()));
    }

    bool CelestiaAppCore::StartSimulation(hstring const&configFileName, array_view<hstring const> extraDirectories, CelestiaComponent::CelestiaLoadCallback const& callback)
    {
        AppCoreProgressWatcher watcher(callback);
        string config = to_string(configFileName);
        vector<fs::path> extraPaths;

        for (unsigned int i = 0; i < extraDirectories.size(); i++)
        {
            extraPaths.emplace_back(to_string(extraDirectories[i]));
        }
        return core->initSimulation(config, extraPaths, &watcher);
    }

    bool CelestiaAppCore::StartRenderer()
    {
        bool success = core->initRenderer();

        // start with default values
        constexpr int DEFAULT_ORBIT_MASK = Body::Planet | Body::Moon | Body::Stellar;
        constexpr int DEFAULT_LABEL_MODE = 2176;
        constexpr float DEFAULT_AMBIENT_LIGHT_LEVEL = 0.1f;
        constexpr float DEFAULT_VISUAL_MAGNITUDE = 8.0f;
        constexpr Renderer::StarStyle DEFAULT_STAR_STYLE = Renderer::FuzzyPointStars;
        constexpr ColorTableType DEFAULT_STARS_COLOR = ColorTableType::Blackbody_D65;
        constexpr unsigned int DEFAULT_TEXTURE_RESOLUTION = medres;
        constexpr float DEFAULT_TINT_SATURATION = 0.5f;

        core->getRenderer()->setRenderFlags(Renderer::DefaultRenderFlags);
        core->getRenderer()->setOrbitMask(DEFAULT_ORBIT_MASK);
        core->getRenderer()->setLabelMode(DEFAULT_LABEL_MODE);
        core->getRenderer()->setAmbientLightLevel(DEFAULT_AMBIENT_LIGHT_LEVEL);
        core->getRenderer()->setTintSaturation(DEFAULT_TINT_SATURATION);
        core->getRenderer()->setStarStyle(DEFAULT_STAR_STYLE);
        core->getRenderer()->setResolution(DEFAULT_TEXTURE_RESOLUTION);
        core->getRenderer()->setStarColorTable(DEFAULT_STARS_COLOR);

        core->getSimulation()->setFaintestVisible(DEFAULT_VISUAL_MAGNITUDE);

        core->getRenderer()->setSolarSystemMaxDistance((core->getConfig()->renderDetails.SolarSystemMaxDistance));
        core->getRenderer()->setShadowMapSize(core->getConfig()->renderDetails.ShadowMapSize);

        return success;
    }

    void CelestiaAppCore::Tick()
    {
        core->tick();
    }

    void CelestiaAppCore::Draw()
    {
        core->draw();
    }

    void CelestiaAppCore::Resize(int32_t width, int32_t height)
    {
        core->resize(width, height);
    }

    int32_t CelestiaAppCore::GetWidth()
    {
        auto [width, _] = core->getWindowDimension();
        return static_cast<int32_t>(width);
    }

    int32_t CelestiaAppCore::GetHeight()
    {
        auto [_, height] = core->getWindowDimension();
        return static_cast<int32_t>(height);
    }

    void CelestiaAppCore::Start()
    {
        core->start();
    }

    void CelestiaAppCore::SetDPI(int32_t dpi)
    {
        core->setScreenDpi(dpi);
    }

    void CelestiaAppCore::SetSafeAreaInsets(int32_t left, int32_t top, int32_t right, int32_t bottom)
    {
        core->setSafeAreaInsets(left, top, right, bottom);
    }

    void CelestiaAppCore::MouseMove(float x, float y, CelestiaComponent::CelestiaMouseButton button)
    {
        core->mouseMove(x, y, (int)button);
    }

    void CelestiaAppCore::MouseButtonUp(float x, float y, CelestiaComponent::CelestiaMouseButton button)
    {
        core->mouseButtonUp(x, y, (int)button);
    }

    void CelestiaAppCore::MouseButtonDown(float x, float y, CelestiaComponent::CelestiaMouseButton button)
    {
        core->mouseButtonDown(x, y, (int)button);
    }

    void CelestiaAppCore::MouseWheel(float motion, int32_t modifiers)
    {
        core->mouseWheel(motion, modifiers);
    }

    void CelestiaAppCore::CharEnter(int16_t input)
    {
        core->charEntered((char)input);
    }

    void CelestiaAppCore::CharEnter(int16_t input, int32_t modifiers)
    {
        core->charEntered((char)input, modifiers);
    }

    static int convert_key_code_to_celestia_key(int key)
    {
        int celestiaKey = -1;
        if (key >= (int)Windows::System::VirtualKey::NumberPad0 && key <= (int)Windows::System::VirtualKey::NumberPad9)
            celestiaKey = CelestiaCore::Key_NumPad0 + (key - (int)Windows::System::VirtualKey::NumberPad0);
        else if (key >= (int)Windows::System::VirtualKey::F1 && key <= (int)Windows::System::VirtualKey::F12)
            celestiaKey = CelestiaCore::Key_F1 + (key - (int)Windows::System::VirtualKey::F1);
        else if (key >= (int)Windows::System::VirtualKey::A && key <= (int)Windows::System::VirtualKey::Z)
            celestiaKey = 'A' + (key - (int)Windows::System::VirtualKey::A);
        else
            switch (key)
            {
            case (int)Windows::System::VirtualKey::Up:
                celestiaKey = CelestiaCore::Key_Up;
                break;
            case (int)Windows::System::VirtualKey::Down:
                celestiaKey = CelestiaCore::Key_Down;
                break;
            case (int)Windows::System::VirtualKey::Left:
                celestiaKey = CelestiaCore::Key_Left;
                break;
            case (int)Windows::System::VirtualKey::Right:
                celestiaKey = CelestiaCore::Key_Right;
                break;
            case (int)Windows::System::VirtualKey::PageUp:
                celestiaKey = CelestiaCore::Key_PageUp;
                break;
            case (int)Windows::System::VirtualKey::PageDown:
                celestiaKey = CelestiaCore::Key_PageDown;
                break;
            case (int)Windows::System::VirtualKey::Home:
                celestiaKey = CelestiaCore::Key_Home;
                break;
            case (int)Windows::System::VirtualKey::End:
                celestiaKey = CelestiaCore::Key_End;
                break;
            case (int)Windows::System::VirtualKey::Insert:
                celestiaKey = CelestiaCore::Key_Insert;
                break;
            case (int)Windows::System::VirtualKey::Back:
                celestiaKey = celestiaKey;
                break;
            case (int)Windows::System::VirtualKey::Delete:
                celestiaKey = 127;
                break;
            default:
                break;
            }

        return celestiaKey;
    }

    void CelestiaAppCore::KeyUp(int32_t key, int32_t modifiers)
    {
        int celestiaKey = convert_key_code_to_celestia_key(key);
        if (celestiaKey < 0) return;
        core->keyUp(celestiaKey, modifiers);
    }

    void CelestiaAppCore::KeyDown(int32_t key, int32_t modifiers)
    {
        int celestiaKey = convert_key_code_to_celestia_key(key);
        if (celestiaKey < 0) return;
        core->keyDown(celestiaKey, modifiers);
    }

    void CelestiaAppCore::JoystickButtonDown(CelestiaComponent::CelestiaJoystickButton button)
    {
        core->joystickButton((int)button, true);
    }

    void CelestiaAppCore::JoystickButtonUp(CelestiaComponent::CelestiaJoystickButton button)
    {
        core->joystickButton((int)button, false);
    }

    void CelestiaAppCore::JoystickAxis(CelestiaComponent::CelestiaJoystickAxis axis, float amount)
    {
        core->joystickAxis((int)axis, amount);
    }

    void CelestiaAppCore::SetHudFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize)
    {
        core->setHudFont(to_string(fontPath), collectionIndex, fontSize);
    }

    void CelestiaAppCore::SetHudTitleFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize)
    {
        core->setHudTitleFont(to_string(fontPath), collectionIndex, fontSize);
    }

    void CelestiaAppCore::SetRenderFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize, CelestiaComponent::CelestiaFontStyle fontStyle)
    {
        core->setRendererFont(to_string(fontPath), collectionIndex, fontSize, (Renderer::FontStyle)fontStyle);
    }

    void CelestiaAppCore::RunScript(hstring const& path)
    {
        core->runScript(to_string(path));
    }

    void CelestiaAppCore::RunDemo()
    {
        const auto& demoScriptFile = core->getConfig()->paths.demoScriptFile;
        if (!demoScriptFile.empty())
        {
            core->cancelScript();
            core->runScript(demoScriptFile);
        }
    }

    void CelestiaAppCore::GoToURL(hstring const& url)
    {
        core->goToUrl(to_string(url));
    }

    bool CelestiaAppCore::SaveScreenshot(hstring const& filePath)
    {
        return core->saveScreenShot(to_string(filePath), ContentType::PNG);
    }

    com_array<CelestiaComponent::CelestiaDestination> CelestiaAppCore::Destinations()
    {
        auto destinations = core->getDestinations();
        std::vector<CelestiaComponent::CelestiaDestination> vector;
        for (unsigned int i = 0; i < destinations->size(); ++i)
        {
            vector.push_back(make<implementation::CelestiaDestination>(destinations->at(i)));
        }
        return com_array(vector);
    }

    CelestiaComponent::CelestiaSimulation CelestiaAppCore::Simulation()
    {
        if (sim == nullptr)
            sim = make<implementation::CelestiaSimulation>(core->getSimulation());
        return sim;
    }

    hstring CelestiaAppCore::CurrentURL()
    {
        CelestiaState appState(core);
        appState.captureState();

        Url currentURL(appState, Url::CurrentVersion);
        return to_hstring(currentURL.getAsString());
    }

    hstring CelestiaAppCore::RenderInfo()
    {
        return to_hstring(Helper::getRenderInfo(core->getRenderer()));
    }

    int64_t CelestiaAppCore::Pointer()
    {
        return reinterpret_cast<int64_t>(core);
    }

    void CelestiaAppCore::InitGL()
    {
        celestia::gl::init();
    }

    hstring CelestiaAppCore::Language()
    {
#ifdef ENABLE_NLS
        const char* lang = dgettext("celestia", "LANGUAGE");
        if (strcmp(lang, "LANGUAGE") == 0)
            return L"en";
        return to_hstring(lang);
#else
        return L"en";
#endif
    }

    void CelestiaAppCore::SetLocaleDirectory(hstring const& localeDirectory, hstring const& locale)
    {
        UErrorCode status = U_ZERO_ERROR;
        uloc_setDefault(to_string(locale).c_str(), &status);
#ifdef ENABLE_NLS
        _wputenv_s(L"LANG", locale.c_str());
        wbindtextdomain("celestia", localeDirectory.c_str());
        bind_textdomain_codeset("celestia", "UTF-8");
        wbindtextdomain("celestia-data", localeDirectory.c_str());
        bind_textdomain_codeset("celestia-data", "UTF-8");
        wbindtextdomain("celestia_ui", localeDirectory.c_str());
        bind_textdomain_codeset("celestia_ui", "UTF-8");
        textdomain("celestia");
#endif
    }

    void CelestiaAppCore::SetUpLocale()
    {
#ifdef ENABLE_NLS
        setlocale(LC_ALL, "");
        setlocale(LC_NUMERIC, "C");
#endif
    }

    hstring CelestiaAppCore::LocalizedString(hstring const& original, hstring const& domain)
    {
#ifdef ENABLE_NLS
        if (original.empty())
            return original;
        return to_hstring(dgettext(to_string(domain).c_str(), to_string(original).c_str()));
#else
        return original;
#endif
    }

    hstring CelestiaAppCore::LocalizedString(hstring const& original, hstring const& context, hstring const& domain)
    {
#ifdef ENABLE_NLS
        if (original.empty())
            return original;
        std::string auxStr = fmt::format("{}\004{}", to_string(context).c_str(), to_string(original).c_str());
        const char *aux = auxStr.c_str();
        const char *translation = dgettext(to_string(domain).c_str(), aux);
        return translation == aux ? original : to_hstring(translation);
#else
        return original;
#endif
    }

    com_array<CelestiaComponent::CelestiaScript> CelestiaAppCore::ReadScripts(hstring const& directory, bool deepScan)
    {
        auto results = ScanScriptsDirectory(to_string(directory), deepScan ? true : false);
        std::vector<CelestiaComponent::CelestiaScript> vector;
        for (const auto &result : results)
        {
            vector.push_back(make<implementation::CelestiaScript>(result));
        }
        return com_array(vector);
    }


    static uint64_t bit_mask_value_update(bool value, uint64_t bit, uint64_t set)
    {
        uint64_t result = value ? ((bit & set) ? set : (set | bit)) : ((bit & set) ? (set ^ bit) : set);
        return result;
    }

#define RENDERMETHODS(flag) \
bool CelestiaAppCore::Show##flag() \
{ \
    return (core->getRenderer()->getRenderFlags() & Renderer::Show##flag) != 0; \
} \
void CelestiaAppCore::Show##flag(bool value) \
{ \
    core->getRenderer()->setRenderFlags(bit_mask_value_update(value, Renderer::Show##flag, core->getRenderer()->getRenderFlags())); \
} \

    RENDERMETHODS(Stars)
    RENDERMETHODS(Planets)
    RENDERMETHODS(DwarfPlanets)
    RENDERMETHODS(Moons)
    RENDERMETHODS(MinorMoons)
    RENDERMETHODS(Asteroids)
    RENDERMETHODS(Comets)
    RENDERMETHODS(Spacecrafts)
    RENDERMETHODS(Galaxies)
    RENDERMETHODS(Globulars)
    RENDERMETHODS(Nebulae)
    RENDERMETHODS(OpenClusters)
    RENDERMETHODS(Diagrams)
    RENDERMETHODS(Boundaries)
    RENDERMETHODS(CloudMaps)
    RENDERMETHODS(NightMaps)
    RENDERMETHODS(Atmospheres)
    RENDERMETHODS(CometTails)
    RENDERMETHODS(PlanetRings)
    RENDERMETHODS(Markers)
    RENDERMETHODS(Orbits)
    RENDERMETHODS(FadingOrbits)
    RENDERMETHODS(Ecliptic)
    RENDERMETHODS(PartialTrajectories)
    RENDERMETHODS(SmoothLines)
    RENDERMETHODS(EclipseShadows)
    RENDERMETHODS(RingShadows)
    RENDERMETHODS(CloudShadows)
    RENDERMETHODS(AutoMag)
    RENDERMETHODS(CelestialSphere)
    RENDERMETHODS(EclipticGrid)
    RENDERMETHODS(HorizonGrid)
    RENDERMETHODS(GalacticGrid)

#define LABELMETHODS(flag) \
bool CelestiaAppCore::Show##flag##Labels() \
{ \
    return (core->getRenderer()->getLabelMode() & Renderer::flag##Labels) != 0; \
} \
void CelestiaAppCore::Show##flag##Labels(bool value) \
{ \
    core->getRenderer()->setLabelMode((int)bit_mask_value_update(value, Renderer::flag##Labels, core->getRenderer()->getLabelMode())); \
}
    LABELMETHODS(Star)
    LABELMETHODS(Planet)
    LABELMETHODS(Moon)
    LABELMETHODS(Constellation)
    LABELMETHODS(Galaxy)
    LABELMETHODS(Globular)
    LABELMETHODS(Nebula)
    LABELMETHODS(OpenCluster)
    LABELMETHODS(Asteroid)
    LABELMETHODS(Spacecraft)
    LABELMETHODS(Location)
    LABELMETHODS(Comet)
    LABELMETHODS(DwarfPlanet)
    LABELMETHODS(MinorMoon)
    LABELMETHODS(I18nConstellation)


    bool CelestiaAppCore::ShowLatinConstellationLabels()
    {
        return !ShowI18nConstellationLabels();
    }

    void CelestiaAppCore::ShowLatinConstellationLabels(bool showLatinConstellationLabels)
    {
        ShowI18nConstellationLabels(!showLatinConstellationLabels);
    }

#define ORBITMETHODS(flag) \
bool CelestiaAppCore::Show##flag##Orbits() \
{ \
    return (core->getRenderer()->getOrbitMask() & Body::flag) != 0; \
} \
void CelestiaAppCore::Show##flag##Orbits(bool value) \
{ \
    core->getRenderer()->setOrbitMask((int)bit_mask_value_update(value, Body::flag, core->getRenderer()->getOrbitMask())); \
}

    ORBITMETHODS(Planet)
    ORBITMETHODS(Moon)
    ORBITMETHODS(Asteroid)
    ORBITMETHODS(Spacecraft)
    ORBITMETHODS(Comet)
    ORBITMETHODS(Stellar)
    ORBITMETHODS(DwarfPlanet)
    ORBITMETHODS(MinorMoon)

#define FEATUREMETHODS(flag) \
bool CelestiaAppCore::Show##flag##Labels() \
{ \
    return (core->getSimulation()->getObserver().getLocationFilter() & Location::flag) != 0; \
} \
void CelestiaAppCore::Show##flag##Labels(bool value) \
{ \
    core->getSimulation()->getObserver().setLocationFilter((int)bit_mask_value_update(value, Location::flag, core->getSimulation()->getObserver().getLocationFilter())); \
}
    FEATUREMETHODS(City)
    FEATUREMETHODS(Observatory)
    FEATUREMETHODS(LandingSite)
    FEATUREMETHODS(Crater)
    FEATUREMETHODS(Vallis)
    FEATUREMETHODS(Mons)
    FEATUREMETHODS(Planum)
    FEATUREMETHODS(Chasma)
    FEATUREMETHODS(Patera)
    FEATUREMETHODS(Mare)
    FEATUREMETHODS(Rupes)
    FEATUREMETHODS(Tessera)
    FEATUREMETHODS(Regio)
    FEATUREMETHODS(Chaos)
    FEATUREMETHODS(Terra)
    FEATUREMETHODS(Astrum)
    FEATUREMETHODS(Corona)
    FEATUREMETHODS(Dorsum)
    FEATUREMETHODS(Fossa)
    FEATUREMETHODS(Catena)
    FEATUREMETHODS(Mensa)
    FEATUREMETHODS(Rima)
    FEATUREMETHODS(Undae)
    FEATUREMETHODS(Reticulum)
    FEATUREMETHODS(Planitia)
    FEATUREMETHODS(Linea)
    FEATUREMETHODS(Fluctus)
    FEATUREMETHODS(Farrum)
    FEATUREMETHODS(EruptiveCenter)
    FEATUREMETHODS(Other)

#define INTERACTIONMETHODS(flag) \
bool CelestiaAppCore::Enable##flag() \
{ \
    return celestia::util::is_set(core->getInteractionFlags(), CelestiaCore::InteractionFlags::flag); \
} \
void CelestiaAppCore::Enable##flag(bool value) \
{ \
    auto flags = core->getInteractionFlags(); \
    celestia::util::set_or_unset(flags, CelestiaCore::InteractionFlags::flag, value); \
    core->setInteractionFlags(flags); \
}

    INTERACTIONMETHODS(ReverseWheel)
    INTERACTIONMETHODS(RayBasedDragging)
    INTERACTIONMETHODS(FocusZooming)

    int32_t CelestiaAppCore::Resolution()
    {
        return core->getRenderer()->getResolution();
    }

    void CelestiaAppCore::Resolution(int32_t resolution)
    {
        core->getRenderer()->setResolution(resolution);
    }

    int32_t CelestiaAppCore::StarStyle()
    {
        return (int32_t)core->getRenderer()->getStarStyle();
    }

    void CelestiaAppCore::StarStyle(int32_t starStyle)
    {
        core->getRenderer()->setStarStyle((Renderer::StarStyle)starStyle);
    }

    int32_t CelestiaAppCore::StarColors()
    {
        return static_cast<int32_t>(core->getRenderer()->getStarColorTable());
    }

    void CelestiaAppCore::StarColors(int32_t starColors)
    {
        core->getRenderer()->setStarColorTable(static_cast<ColorTableType>(starColors));
    }

    int32_t CelestiaAppCore::HudDetail()
    {
        return core->getHudDetail();
    }

    void CelestiaAppCore::HudDetail(int32_t hudDetail)
    {
        core->setHudDetail(hudDetail);
    }

    int32_t CelestiaAppCore::DateFormat()
    {
        return static_cast<int32_t>(core->getDateFormat());
    }

    void CelestiaAppCore::DateFormat(int32_t dateFormat)
    {
        core->setDateFormat(static_cast<celestia::astro::Date::Format>(dateFormat));
    }

    float CelestiaAppCore::AmbientLightLevel()
    {
        return core->getRenderer()->getAmbientLightLevel();
    }

    void CelestiaAppCore::AmbientLightLevel(float ambientLightLevel)
    {
        core->getRenderer()->setAmbientLightLevel(ambientLightLevel);
    }

    float CelestiaAppCore::FaintestVisible()
    {
        if ((core->getRenderer()->getRenderFlags() & Renderer::ShowAutoMag) == 0)
        {
            return core->getSimulation()->getFaintestVisible();
        }
        else
        {
            return core->getRenderer()->getFaintestAM45deg();
        }
    }

    void CelestiaAppCore::FaintestVisible(float faintestVisible)
    {
        if ((core->getRenderer()->getRenderFlags() & Renderer::ShowAutoMag) == 0)
        {
            core->setFaintest(faintestVisible);
        }
        else
        {
            core->getRenderer()->setFaintestAM45deg(faintestVisible);
            core->setFaintestAutoMag();
        }
    }

    float CelestiaAppCore::GalaxyBrightness()
    {
        return Galaxy::getLightGain();
    }

    void CelestiaAppCore::GalaxyBrightness(float galaxyBrightness)
    {
        Galaxy::setLightGain(galaxyBrightness);
    }

    float CelestiaAppCore::MinimumFeatureSize()
    {
        return core->getRenderer()->getMinimumFeatureSize();
    }

    void CelestiaAppCore::MinimumFeatureSize(float minimumFeatureSize)
    {
        core->getRenderer()->setMinimumFeatureSize(minimumFeatureSize);
    }

    float CelestiaAppCore::DistanceLimit()
    {
        return core->getRenderer()->getDistanceLimit();
    }

    void CelestiaAppCore::DistanceLimit(float distanceLimit)
    {
        core->getRenderer()->setDistanceLimit(distanceLimit);
    }

    float CelestiaAppCore::TintSaturation()
    {
        return core->getRenderer()->getTintSaturation();
    }

    void CelestiaAppCore::TintSaturation(float tintSaturation)
    {
        core->getRenderer()->setTintSaturation(tintSaturation);
    }

    bool CelestiaAppCore::ShowBodyAxes()
    {
        return core->referenceMarkEnabled("body axes");
    }

    void CelestiaAppCore::ShowBodyAxes(bool showBodyAxes)
    {
        core->toggleReferenceMark("body axes");
    }

    bool CelestiaAppCore::ShowFrameAxes()
    {
        return core->referenceMarkEnabled("frame axes");
    }

    void CelestiaAppCore::ShowFrameAxes(bool showFrameAxes)
    {
        core->toggleReferenceMark("frame axes");
    }

    bool CelestiaAppCore::ShowSunDirection()
    {
        return core->referenceMarkEnabled("sun direction");
    }

    void CelestiaAppCore::ShowSunDirection(bool showSunDirection)
    {
        core->toggleReferenceMark("sun direction");
    }

    bool CelestiaAppCore::ShowVelocityVector()
    {
        return core->referenceMarkEnabled("velocity vector");
    }

    void CelestiaAppCore::ShowVelocityVector(bool showVelocityVector)
    {
        core->toggleReferenceMark("velocity vector");
    }

    bool CelestiaAppCore::ShowPlanetographicGrid()
    {
        return core->referenceMarkEnabled("planetographic grid");
    }

    void CelestiaAppCore::ShowPlanetographicGrid(bool showPlanetographicGrid)
    {
        core->toggleReferenceMark("planetographic grid");
    }

    bool CelestiaAppCore::ShowTerminator()
    {
        return core->referenceMarkEnabled("terminator");
    }

    void CelestiaAppCore::ShowTerminator(bool showTerminator)
    {
        core->toggleReferenceMark("terminator");
    }

    int32_t CelestiaAppCore::TimeZone()
    {
        return core->getTimeZoneBias() == 0 ? 1 : 0;
    }

    void CelestiaAppCore::TimeZone(int32_t timeZone)
    {
        core->setTimeZoneBias(0 == timeZone ? 1 : 0);
    }

    int32_t CelestiaAppCore::MeasurementSystem()
    {
        return (int32_t)core->getMeasurementSystem();
    }

    void CelestiaAppCore::MeasurementSystem(int32_t measurementSystem)
    {
        core->setMeasurementSystem(static_cast<celestia::MeasurementSystem>(measurementSystem));
    }

    int32_t CelestiaAppCore::TemperatureScale()
    {
        return (int32_t)core->getTemperatureScale();
    }

    void CelestiaAppCore::TemperatureScale(int32_t temperatureScale)
    {
        core->setTemperatureScale(static_cast<celestia::TemperatureScale>(temperatureScale));
    }

    int32_t CelestiaAppCore::ScriptSystemAccessPolicy()
    {
        return (int32_t)core->getScriptSystemAccessPolicy();
    }

    void CelestiaAppCore::ScriptSystemAccessPolicy(int32_t scriptSystemAccessPolicy)
    {
        core->setScriptSystemAccessPolicy((CelestiaCore::ScriptSystemAccessPolicy)scriptSystemAccessPolicy);
    }

    CelestiaComponent::CelestiaLayoutDirection CelestiaAppCore::LayoutDirection()
    {
        return (CelestiaComponent::CelestiaLayoutDirection)core->getLayoutDirection();
    }

    void CelestiaAppCore::LayoutDirection(CelestiaComponent::CelestiaLayoutDirection layoutDirection)
    {
        core->setLayoutDirection(static_cast<celestia::LayoutDirection>(layoutDirection));
    }

    float CelestiaAppCore::PickTolerance()
    {
        return 1.0f;
    }

    void CelestiaAppCore::PickTolerance(float pickTolerance)
    {
        core->setPickTolerance(pickTolerance);
    }

    event_token CelestiaAppCore::ShowContextMenu(Windows::Foundation::EventHandler<CelestiaComponent::ShowContextMenuArgs> const& handler)
    {
        return showContextMenuEvent.add(handler);
    }

    void CelestiaAppCore::ShowContextMenu(event_token const& token)
    {
        showContextMenuEvent.remove(token);
    }

    event_token CelestiaAppCore::ChangeCursor(Windows::Foundation::EventHandler<CelestiaComponent::ChangeCursorArgs> const& handler)
    {
        return changeCursorEvent.add(handler);
    }

    void CelestiaAppCore::ChangeCursor(event_token const& token)
    {
        changeCursorEvent.remove(token);
    }

    event_token CelestiaAppCore::FatalError(Windows::Foundation::EventHandler<CelestiaComponent::FatalErrorArgs> const& handler)
    {
        return fatalErrorEvent.add(handler);
    }

    void CelestiaAppCore::FatalError(event_token const& token)
    {
        fatalErrorEvent.remove(token);
    }
}
