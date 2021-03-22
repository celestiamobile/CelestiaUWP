#include "pch.h"
#include <celestia/celestiastate.h>
#include <celestia/url.h>
#include <celestia/helper.h>
#ifdef ENABLE_NLS
#include <celutil/gettext.h>
#endif
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
        AppCoreContextMenuHandler(CelestiaComponent::CelestiaContextMenuCallback const& handler) : CelestiaCore::ContextMenuHandler(), handler(handler) {};

        void requestContextMenu(float x, float y, Selection sel)
        {
            handler(x, y, make<CelestiaSelection>(sel));
        }
    private:
        CelestiaComponent::CelestiaContextMenuCallback handler;
    };

    CelestiaAppCore::CelestiaAppCore() : CelestiaAppCoreT<CelestiaAppCore>(), sim(nullptr)
    {
        core = new CelestiaCore;
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
        const int DEFAULT_ORBIT_MASK = Body::Planet | Body::Moon | Body::Stellar;
        const int DEFAULT_LABEL_MODE = 2176;
        const float DEFAULT_AMBIENT_LIGHT_LEVEL = 0.1f;
        const float DEFAULT_VISUAL_MAGNITUDE = 8.0f;
        const Renderer::StarStyle DEFAULT_STAR_STYLE = Renderer::FuzzyPointStars;
        const ColorTableType DEFAULT_STARS_COLOR = ColorTable_Blackbody_D65;
        const unsigned int DEFAULT_TEXTURE_RESOLUTION = medres;

        core->getRenderer()->setRenderFlags(Renderer::DefaultRenderFlags);
        core->getRenderer()->setOrbitMask(DEFAULT_ORBIT_MASK);
        core->getRenderer()->setLabelMode(DEFAULT_LABEL_MODE);
        core->getRenderer()->setAmbientLightLevel(DEFAULT_AMBIENT_LIGHT_LEVEL);
        core->getRenderer()->setStarStyle(DEFAULT_STAR_STYLE);
        core->getRenderer()->setResolution(DEFAULT_TEXTURE_RESOLUTION);
        core->getRenderer()->setStarColorTable(GetStarColorTable(DEFAULT_STARS_COLOR));

        core->getSimulation()->setFaintestVisible(DEFAULT_VISUAL_MAGNITUDE);

        core->getRenderer()->setSolarSystemMaxDistance((core->getConfig()->SolarSystemMaxDistance));

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

    void CelestiaAppCore::Start()
    {
        core->start();
    }

    void CelestiaAppCore::SetDPI(int32_t dpi)
    {
        core->setScreenDpi(dpi);
    }

    void CelestiaAppCore::MouseMove(float x, float y, int32_t button)
    {
        core->mouseMove(x, y, button);
    }

    void CelestiaAppCore::MouseButtonUp(float x, float y, int32_t button)
    {
        core->mouseButtonUp(x, y, button);
    }

    void CelestiaAppCore::MouseButtonDown(float x, float y, int32_t button)
    {
        core->mouseButtonDown(x, y, button);
    }

    void CelestiaAppCore::CharEnter(int16_t input)
    {
        core->charEntered(input);
    }

    void CelestiaAppCore::SetContextMenuHandler(CelestiaComponent::CelestiaContextMenuCallback const& handler)
    {
        auto previousHandler = core->getContextMenuHandler();
        if (previousHandler)
            delete previousHandler;

        core->setContextMenuHandler(new AppCoreContextMenuHandler(handler));
    }

    void CelestiaAppCore::SetFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize)
    {
        core->setFont(to_string(fontPath), collectionIndex, fontSize);
    }

    void CelestiaAppCore::SetTitleFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize)
    {
        core->setTitleFont(to_string(fontPath), collectionIndex, fontSize);
    }

    void CelestiaAppCore::SetRenderFont(hstring const& fontPath, int32_t collectionIndex, int32_t fontSize, CelestiaComponent::CelestiaFontStyle fontStyle)
    {
        core->setRendererFont(to_string(fontPath), collectionIndex, fontSize, (Renderer::FontStyle)fontStyle);
    }

    void CelestiaAppCore::RunScript(hstring const& path)
    {
        core->runScript(to_string(path));
    }

    void CelestiaAppCore::GoToURL(hstring const& url)
    {
        core->goToUrl(to_string(url));
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

    void CelestiaAppCore::SetLocaleDirectory(hstring const& localeDirectory, hstring const& locale)
    {
#ifdef ENABLE_NLS
        _putenv_s("LANGUAGE", to_string(locale).c_str());
        std::string dir = to_string(localeDirectory);
        // Gettext integration
        setlocale(LC_ALL, "");
        setlocale(LC_NUMERIC, "C");
        bindtextdomain("celestia", dir.c_str());
        bind_textdomain_codeset("celestia", "UTF-8");
        bindtextdomain("celestia_constellations", dir.c_str());
        bind_textdomain_codeset("celestia_constellations", "UTF-8");
        bindtextdomain("celestia_ui", dir.c_str());
        bind_textdomain_codeset("celestia_ui", "UTF-8");
        textdomain("celestia");
#endif
    }

    hstring CelestiaAppCore::LocalizedString(hstring const& original)
    {
        return original;
    }

    com_array<CelestiaComponent::CelestiaScript> CelestiaAppCore::ReadScripts(hstring const& directory, bool deepScan)
    {
        std::vector<ScriptMenuItem>* results = ScanScriptsDirectory(to_string(directory), deepScan ? true : false);
        std::vector<CelestiaComponent::CelestiaScript> vector;
        for (unsigned int i = 0; i < results->size(); ++i)
        {
            auto result = (*results)[i];
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
        return (int32_t)core->getDateFormat();
    }

    void CelestiaAppCore::DateFormat(int32_t dateFormat)
    {
        core->setDateFormat((astro::Date::Format)dateFormat);
    }

    double CelestiaAppCore::AmbientLightLevel()
    {
        return core->getRenderer()->getAmbientLightLevel();
    }

    void CelestiaAppCore::AmbientLightLevel(double ambientLightLevel)
    {
        core->getRenderer()->setAmbientLightLevel((float)ambientLightLevel);
    }

    double CelestiaAppCore::FaintestVisible()
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

    void CelestiaAppCore::FaintestVisible(double faintestVisible)
    {
        if ((core->getRenderer()->getRenderFlags() & Renderer::ShowAutoMag) == 0)
        {
            core->setFaintest((float)faintestVisible);
        }
        else
        {
            core->getRenderer()->setFaintestAM45deg((float)faintestVisible);
            core->setFaintestAutoMag();
        }
    }

    double CelestiaAppCore::GalaxyBrightness()
    {
        return Galaxy::getLightGain();
    }

    void CelestiaAppCore::GalaxyBrightness(double galaxyBrightness)
    {
        Galaxy::setLightGain((float)galaxyBrightness);
    }

    double CelestiaAppCore::MinimumFeatureSize()
    {
        return core->getRenderer()->getMinimumFeatureSize();
    }

    void CelestiaAppCore::MinimumFeatureSize(double minimumFeatureSize)
    {
        core->getRenderer()->setMinimumFeatureSize((float)minimumFeatureSize);
    }
}
