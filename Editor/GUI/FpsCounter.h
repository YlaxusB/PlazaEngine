#pragma once
#include "Engine/Core/Time.h"
namespace Plaza::Editor {
	class FpsCounter {
	public:
		FpsCounter() {

		}

		void Update() {
			PLAZA_PROFILE_SECTION("Update FPS Counter");
			ImGuiWindowFlags windowflags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing;
			windowflags |= ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
			bool showFPSCounter = true;
			if (ImGui::Begin("FPS Overlay", &showFPSCounter, windowflags))
			{
				ImGui::SetWindowPos(ImVec2(Application->appSizes->sceneStart.x + Application->appSizes->sceneSize.x + (ImGui::GetWindowSize().x / 2), Application->appSizes->sceneStart.y + ImGui::GetWindowPos().y), ImGuiCond_Once);
				ImGui::Text(("MS/Frame:" + std::to_string(Time::msPerFrame)).c_str());
				ImGui::Text(("FPS:" + std::to_string(Time::fps)).c_str());
				ImGui::Text(("Scene Size X:" + std::to_string(Application->appSizes->sceneSize.x)).c_str());
				ImGui::Text(("Scene Size Y:" + std::to_string(Application->appSizes->sceneSize.y)).c_str());
				ImGui::Text(("Scene Start X:" + std::to_string(Application->appSizes->sceneStart.x)).c_str());
				ImGui::Text(("Scene Start Y:" + std::to_string(Application->appSizes->sceneStart.y)).c_str());
				ImGui::Text(("Draw Calls:" + std::to_string(Time::drawCalls)).c_str());
				ImGui::Text(("Add Instance Calls:" + std::to_string(Time::addInstanceCalls)).c_str());
				ImGui::Text(("Debug Layer: " + std::to_string(Application->Shadows->debugLayer)).c_str());
				unsigned int i = 0;
				for (float value : Application->Shadows->shadowCascadeLevels) {
					ImGui::Text(("Cascade level" + std::to_string(i) + ": " + std::to_string(value)).c_str());
					i++;
				}




				ImGui::End();
			}
		}
	};
}