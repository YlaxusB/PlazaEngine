#pragma once
#include "Engine/Core/Renderer/Renderer.h"
namespace Plaza::Editor{
	static class Settings {
	public:
		static std::string name;
		static bool vsync;
		static RendererAPI mDefaultRendererAPI;

		static void ReapplyAllSettings();
	};
}

inline bool Plaza::Editor::Settings::vsync = false; 
inline std::string Plaza::Editor::Settings::name = "editouri";
inline RendererAPI Plaza::Editor::Settings::mDefaultRendererAPI = RendererAPI::Vulkan;