#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
namespace Plaza::Editor {
	static class CppScriptComponentInspector {
	public:
		CppScriptComponentInspector(CsScriptComponent* scriptComponent) {
			ImGui::PushID("CppScriptComponentInspector");
			ImGui::SetNextItemOpen(true);
			ImVec2 oldCursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 150.0f, ImGui::GetCursorPosY()));
			if (ImGui::Button("Remove Component")) {
				scriptComponent->GetGameObject()->RemoveComponent<CsScriptComponent>();
			}
			ImGui::SameLine();
			ImGui::SetCursorPos(oldCursorPos);
			bool header = ImGui::CollapsingHeader("Scripts", ImGuiTreeNodeFlags_DefaultOpen);
			if (header) {

				ImGui::Text("helo");
				MonoClassField* floatField = mono_class_get_field_from_name(mono_object_get_class(scriptComponent->monoObject), "speed");
				float value;
				mono_field_get_value(scriptComponent->monoObject, floatField, &value);
				if (ImGui::DragFloat("Speed: ", &value)) {
					mono_field_set_value(scriptComponent->monoObject, floatField, &value);
				}
			}
			ImGui::PopID();
		}
	};
}