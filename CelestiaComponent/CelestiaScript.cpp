#include "pch.h"
#include "CelestiaScript.h"
#if __has_include("CelestiaScript.g.cpp")
#include "CelestiaScript.g.cpp"
#endif

using namespace std;

namespace winrt::CelestiaComponent::implementation
{
	CelestiaScript::CelestiaScript(ScriptMenuItem const& script) : CelestiaScriptT<CelestiaScript>(), filename(to_hstring(script.filename.string())), title(to_hstring(script.title))
	{
	}

	hstring CelestiaScript::Filename()
	{
		return filename;
	}

	hstring CelestiaScript::Title()
	{
		return title;
	}
}
