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

void CelestiaAppCore::SetLocaleDirectory(String^ localeDirectory)
{
}