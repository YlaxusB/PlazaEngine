#include "Engine/Core/PreCompiledHeaders.h"
#include "EditorInspector.h"
#include "Editor/GUI/Utils/Utils.h"
#include<iostream>
#include "Editor/Settings/EditorSettings.h"
#include "Editor/Settings/SettingsSerializer.h"

namespace Plaza::Editor {
	void EditorInspector::Update() {
		if (Utils::ComponentInspectorHeader(nullptr, "Editor")) {
			ImGui::PushID("EditorInspector");

			/* Editor Settings */
			if (Utils::ComponentInspectorHeader(nullptr, "Settings")) {
				if (ImGui::Checkbox("VSync", &Editor::Settings::vsync)) {
					Settings::ReapplyAllSettings();
				}

				if (ImGui::Button("Save Settings")) {
					Editor::EditorSettingsSerializer::Serialize();
				}
			}

			/* Colors */
			if (Utils::ComponentInspectorHeader(nullptr, "Colors", NULL)) {
				int index = 0;
				for (auto& color : ImGui::GetStyle().Colors) {
					ImGui::PushID("EditorInspectorlabel" + index);
					float col[3];
					col[0] = color.x;
					col[1] = color.y;
					col[2] = color.z;
					if (ImGui::ColorPicker3(ImGui::GetStyleColorName(index), col)) {
						color.x = col[0];
						color.y = col[1];
						color.z = col[2];
					};
					index++;
					ImGui::PopID();
				}
			}
			ImGui::PopID();
		}
	}
}