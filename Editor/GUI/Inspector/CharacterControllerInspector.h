#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Physics/CharacterController.h"
#include "Editor/GUI/Utils/DataVisualizer.h"
#include "Editor/GUI/Utils/Utils.h"
#include <functional>
#include <iostream>

namespace Plaza::Editor {
	static class CharacterControllerInspector {
	public:
		CharacterControllerInspector(CharacterController* characterController) {
			if (Utils::ComponentInspectorHeader(characterController, "Character Controller Component")) {
				ImGui::PushID("CharacterControllerInspector");
				


				ImGui::PopID();
			}

		}
	};
}