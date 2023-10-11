#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"
namespace Plaza::Editor {
	static class CppScriptComponentInspector {
	public:
		void CreateRespectiveInteractor(MonoObject* monoObject, MonoClassField* field, int& sliderIndex) {
			int type = mono_type_get_type(mono_field_get_type(field));

			if (type == MONO_TYPE_R4 || type == MONO_TYPE_R8) { // Float
				float value;
				mono_field_get_value(monoObject, field, &value);
				if (ImGui::DragFloat("##: " + sliderIndex, &value)) {
					mono_field_set_value(monoObject, field, &value);
				}
			}
			else if (type == MONO_TYPE_I4) { // Int
				int value;
				mono_field_get_value(monoObject, field, &value);
				if (ImGui::DragInt("## " + sliderIndex, &value)) {
					mono_field_set_value(monoObject, field, &value);
				}
			}
			else if (type == MONO_TYPE_CLASS) { // Class
				MonoObject* newMonoObject = nullptr;
				mono_field_get_value(monoObject, field, &newMonoObject);
				if (newMonoObject != nullptr) {
					void* iter = NULL;
					MonoClass* classInstance = mono_object_get_class(newMonoObject);
					MonoClassField* subField;
					while ((subField = mono_class_get_fields(classInstance, &iter)) != NULL) {
						sliderIndex++;
						ImGui::Text(mono_field_get_name(subField));
						ImGui::Text("  Sub  ");
						CreateRespectiveInteractor(newMonoObject, subField, sliderIndex);
					}
				}
			}

			sliderIndex++;
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
				int sliderIndex = 0;
				ImGui::Text("helo");
				// Get the fields of the class
				for (auto& [key, scriptClass] : scriptComponent->scriptClasses) {
					MonoClassField* field = NULL;
					void* iter = NULL;
					std::unordered_map<std::string, uint32_t> classFields;
					if (scriptClass->monoObject->vtable) {
						while ((field = mono_class_get_fields(mono_object_get_class(scriptClass->monoObject), &iter)) != NULL)
						{
							ImGui::Text(mono_field_get_name(field));
							CreateRespectiveInteractor(scriptClass->monoObject, field, sliderIndex);
						}
					}
					else {
						ImGui::Text("Vtable is a nullptr");
					}
				}
			}
			ImGui::PopID();
		}
	};


}