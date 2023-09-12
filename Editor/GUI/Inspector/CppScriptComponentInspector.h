#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
namespace Plaza::Editor {
	static class CppScriptComponentInspector {
	public:
		void CreateRespectiveInteractor(MonoObject* monoObject, MonoClassField* field) {
			int type = mono_type_get_type(mono_field_get_type(field));

			if (type == MONO_TYPE_R4 || type == MONO_TYPE_R8) { // Float
				float value;
				mono_field_get_value(monoObject, field, &value);
				if (ImGui::DragFloat("##: ", &value)) {
					mono_field_set_value(monoObject, field, &value);
				}
			}
			else if (type == MONO_TYPE_I4) { // Int
				int value;
				mono_field_get_value(monoObject, field, &value);
				if (ImGui::DragInt("## ", &value)) {
					mono_field_set_value(monoObject, field, &value);
				}
			}
		}

		//mono_class_get_fields(mono_object_get_class(scriptComponent->monoObject), iter);
		//MonoClassField* floatField = mono_class_get_field_from_name(mono_object_get_class(scriptComponent->monoObject), "speed");
		//float value;
		//mono_field_get_value(scriptComponent->monoObject, floatField, &value);
		//if (ImGui::DragFloat("Speed: ", &value)) {
		//	mono_field_set_value(scriptComponent->monoObject, floatField, &value);
		//}

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
				// Get the fields of the class
				for (auto& [key, scriptClass] : scriptComponent->scriptClasses) {
					MonoClassField* field = NULL;
					void* iter = NULL;
					std::unordered_map<std::string, uint32_t> classFields;
					while ((field = mono_class_get_fields(mono_object_get_class(scriptClass->monoObject), &iter)) != NULL)
					{
						ImGui::Text(mono_field_get_name(field));
						CreateRespectiveInteractor(scriptClass->monoObject, field);
					}
				}
			}
			ImGui::PopID();
		}
	};


}