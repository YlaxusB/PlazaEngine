#pragma once
#include "Engine/Core/Engine.h"
#include "Editor/Project.h"
#include <string>
#include <filesystem>

namespace Plaza {
	class PLAZA_API Scripting {
	public:
		static void LoadProjectCppDll(const Editor::Project& project);
		static void ReloadAllScripts();
		static void UnloadAllScripts();
		static void Update();

		static std::filesystem::path CopyPasteDevelopmentLibraryFiles(const Editor::Project& project);
		static void PasteEngineLibToGameProject(const Editor::Project& project);
		static void UnloadCurrentLoadedCppDll();
	private:
		static bool LoadCppDll(const std::filesystem::path& path);
		static inline HMODULE sCurrentLoadedCppDll;
		static inline int sReloadIndex = 0;
	};
}