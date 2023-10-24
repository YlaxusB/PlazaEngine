#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Core/Camera.h"
#include "Editor/GUI/Utils/Utils.h"

namespace Plaza::Editor {
	static class CameraInspector {
	public:
		CameraInspector(Camera* component) {
			if (Utils::ComponentInspectorHeader(component, "Camera")) {
				ImGui::PushID("CameraInspector");


				ImGui::PopID();
			}

		}
	};
}