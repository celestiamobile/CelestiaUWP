#include "pch.h"
#include "CelestiaAppCore.h"

#include <celengine/glsupport.h>
#include <celutil/gettext.h>

#include "CXStringUtils.h"

using namespace CelestiaComponent;
using namespace Platform;
using namespace std;

class AppCoreProgressWatcher : public ProgressNotifier
{
public:
	AppCoreProgressWatcher(CelestiaLoadCallback^ loadCallback) : ProgressNotifier(), loadCallback(loadCallback) {};

	void update(const std::string& status)
	{
		loadCallback(Std_Str_To_Managed_Str(status));
	}
private:
	CelestiaLoadCallback^ loadCallback;
};

CelestiaAppCore::CelestiaAppCore() : core(new CelestiaCore)
{
}

void CelestiaAppCore::InitGL()
{
	celestia::gl::init();
}

bool CelestiaAppCore::StartSimulation(String^ configFileName, const Platform::Array<String^>^ extraDirectories, CelestiaLoadCallback^ loadCallback)
{
	AppCoreProgressWatcher watcher(loadCallback);
	string config = Managed_Str_To_Std_Str(configFileName);
	vector<fs::path> extraPaths;

	for (int i = 0; i < extraDirectories->Length; i++)
	{
		extraPaths.emplace_back(Managed_Str_To_Std_Str(extraDirectories->get(i)));
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

void CelestiaAppCore::Resize(int width, int height)
{
    core->resize(width, height);
}

void CelestiaAppCore::Start()
{
    core->start();
}

void CelestiaAppCore::SetDPI(int dpi)
{
    core->setScreenDpi(dpi);
}

void CelestiaAppCore::SetLocaleDirectory(String^ localeDirectory)
{
}