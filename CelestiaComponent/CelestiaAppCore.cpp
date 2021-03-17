#include "pch.h"
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

    CelestiaComponent::CelestiaSimulation CelestiaAppCore::Simulation()
    {
        if (sim == nullptr)
            sim = make<implementation::CelestiaSimulation>(core->getSimulation());
        return sim;
    }

    void CelestiaAppCore::InitGL()
    {
        celestia::gl::init();
    }

    void CelestiaAppCore::SetLocaleDirectory(hstring const& localeDirectory)
    {
    }

    hstring CelestiaAppCore::LocalizedString(hstring const& original)
    {
        return original;
    }
}
