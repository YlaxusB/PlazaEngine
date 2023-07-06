#pragma once
#include "Engine/Core/Time.h"
namespace Engine::Editor {
	class FpsCounter {
	public:
		FpsCounter() {
			
		}

		void Update() {
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse;
			if (ImGui::Begin("FPS Overlay", new bool(true), window_flags))
			{
				ImGui::Text(("MS/Frame:" + std::to_string(Time::msPerFrame)).c_str());
				ImGui::Text(("FPS:" + std::to_string(Time::fps)).c_str());
				ImGui::End();
			}
		}
	};
}