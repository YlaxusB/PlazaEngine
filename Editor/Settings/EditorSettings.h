#pragma once
#include "Engine/Core/Renderer/Renderer.h"
namespace Plaza::Editor{
	static class Settings {
	public:
		std::string mName = "editor";
		bool mVsync = false;
		RendererAPI mDefaultRendererAPI = RendererAPI::Vulkan;

		void ReapplyAllSettings();

		template <class Archive>
		void serialize(Archive& archive) {
			archive(mName, mVsync, mDefaultRendererAPI);
		}
	};
}