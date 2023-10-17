#pragma once
namespace Plaza::Editor{
	static class Settings {
	public:
		static std::string name;
		static bool vsync;

		static void ReapplyAllSettings();
	};
}

inline bool Plaza::Editor::Settings::vsync = false;
inline std::string Plaza::Editor::Settings::name = "editouri";