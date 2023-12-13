#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Rendering/Light.h"
#include "Editor/GUI/Utils/DataVisualizer.h"
#include "Editor/GUI/Utils/Utils.h"
#include <functional>
#include <iostream>

namespace Plaza::Editor {
	static class LightInspector {
	public:
		LightInspector(Light* light) {
			if (Utils::ComponentInspectorHeader(light, "Light Component")) {
				ImGui::PushID("LightInspector");
				
				Editor::Utils::DragFloat3("Color: ", light->color, 0.01f);
				Editor::Utils::DragFloat("Radius: ", light->radius, 0.01f);
				Editor::Utils::DragFloat("Cutoff: ", light->cutoff, 0.01f);
				Editor::Utils::DragFloat("Intensity: ", light->intensity, 0.01f);

				ImGui::PopID();
			}

		}
	};
}