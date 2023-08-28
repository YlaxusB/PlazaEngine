#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
namespace Plaza::Editor {
	class ScriptManager {
	public:
		static void NewCsScript(std::string fullPath);
		static void ReloadScriptsAssembly();
	};
}