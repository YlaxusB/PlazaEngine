#pragma once
#include "Engine/GUI/Inspector.h"
#include "Engine/GUI/gizmo.h"
namespace Gui {
	class TransformInspector {
	public:
		TransformInspector(GameObject* gameObject) {
			if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

				glm::vec3 lastScale = gameObject->transform->scale;

				glm::vec3& currentPosition = gameObject->GetComponent<Transform>()->relativePosition;
				glm::vec3& currentRotation = gameObject->GetComponent<Transform>()->rotation;
				glm::vec3& currentScale = gameObject->GetComponent<Transform>()->scale;

				if (currentScale.x != lastScale.x) {

				}

				gameObject->transform->UpdateChildrenTransform();
				gameObject->transform->UpdateChildrenScale();
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
//				ImGui::TreePop();

				ImGui::Text("Scale: ");
				ImGui::PushID("Scale");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("X", &currentScale.x);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("Y", &currentScale.y);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("Z", &currentScale.z);

				ImGui::Text("World Position");
				ImGui::Text(std::to_string(gameObject->transform->worldPosition.x).c_str());
				ImGui::Text(std::to_string(gameObject->transform->worldPosition.y).c_str());
				ImGui::Text(std::to_string(gameObject->transform->worldPosition.z).c_str());
				ImGui::Text("World Position Scaled");
				ImGui::Text(std::to_string(gameObject->transform->worldPosition.x * gameObject->transform->worldScale.x).c_str());
				ImGui::Text(std::to_string(gameObject->transform->worldPosition.y * gameObject->transform->worldScale.y).c_str());
				ImGui::Text(std::to_string(gameObject->transform->worldPosition.z * gameObject->transform->worldScale.z).c_str());
				ImGui::Text("World Scale");
				ImGui::Text(std::to_string(gameObject->transform->worldScale.x).c_str());
				ImGui::Text(std::to_string(gameObject->transform->worldScale.y).c_str());
				ImGui::Text(std::to_string(gameObject->transform->worldScale.z).c_str());

				ImGui::PopID();
				ImGui::TreePop();
			}
		}
	};
}