// CelestiaScript.cpp
//
// Copyright (C) 2025, Celestia Development Team
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

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
