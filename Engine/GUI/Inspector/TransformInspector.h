#pragma once
#include "Engine/GUI/Inspector.h"

namespace Gui {
	class TransformInspector {
	public:
		TransformInspector(GameObject* gameObject) {
			if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
				glm::vec3& currentPosition = gameObject->GetComponent<Transform>()->position;
				glm::vec3& currentRotation = gameObject->GetComponent<Transform>()->rotation;
				ImGui::Text("Position: ");

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("X", &currentPosition.x);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("Y", &currentPosition.y);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("Z", &currentPosition.z);


				ImGui::Text("Rotation: ");
				ImGui::PushID("Rotation");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("X", &currentRotation.x);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("Y", &currentRotation.y);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("Z", &currentRotation.z);
				ImGui::PopID();
				ImGui::TreePop();
			}
		}
	};
}