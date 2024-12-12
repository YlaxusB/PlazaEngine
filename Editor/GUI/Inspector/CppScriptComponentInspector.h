#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Scripting/CppScriptComponent.h"

namespace Plaza::Editor {
	static class CppScriptComponentInspector {
	public:
		CppScriptComponentInspector(CppScriptComponent* scriptComponent) {
			ImGui::PushID("CppScriptComponentInspector");
			if (Utils::ComponentInspectorHeader(scriptComponent, "Cpp Script Component")) {
				ImGui::Text("Cpp Component");
				for (CppScript* script : scriptComponent->mScripts) {
					script->OnUpdateEditorGUI();
				}
			}
			ImGui::PopID();
		}
	};
}