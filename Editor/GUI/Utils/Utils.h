#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
namespace Plaza::Editor::Utils {
	static bool ComponentInspectorHeader(Component* component, std::string name, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen) {
		ImGui::PushID(("ComponentInspectorHeader" + name).c_str());
		if (component) {
			ImGui::SetNextItemOpen(true);
			ImVec2 oldCursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 150.0f, ImGui::GetCursorPosY()));
			if (ImGui::Button("Remove Component")) {
				component->GetGameObject()->RemoveComponent<Collider>();
			}
			ImGui::SameLine();
			ImGui::SetCursorPos(oldCursorPos);
		}
		bool header = ImGui::CollapsingHeader(name.c_str(), flags);
		ImGui::PopID();
		return header;
	}
}