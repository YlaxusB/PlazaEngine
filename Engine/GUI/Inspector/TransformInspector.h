#pragma once
#include <imgui/imgui.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Engine/GUI/Inspector.h"
#include "Engine/GUI/gizmo.h"
namespace Engine::Editor {
	class Gui::TransformInspector {
	public:
		glm::vec3 moveTowards(const glm::vec3 position, const glm::vec3 eulerRotation, glm::vec3& targetPosition) {
			// Convert Euler angles rotation to radians
			float yaw = glm::radians(eulerRotation.y);

			// Calculate the direction vector based on the yaw rotation
			glm::vec3 direction(std::sin(yaw), 0.0f, std::cos(yaw));

			// Normalize the direction vector
			direction = glm::normalize(direction);

			// Define the target position
			//glm::vec3 targetPosition(34.0f, 3.0f, 15.0f);

			// Calculate the displacement vector towards the target position
			glm::vec3 displacement = targetPosition - position;

			// Scale the displacement vector to ensure it doesn't overshoot the target
			float distance = glm::length(displacement);
			glm::vec3 scaledDisplacement = glm::normalize(displacement) * glm::min(distance, 1.0f);

			// Calculate the new position by moving towards the target along the direction vector
			glm::vec3 newPosition = position + scaledDisplacement;
			return newPosition;
		}


		TransformInspector(GameObject* gameObject) {
			if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
				glm::vec3 startingPosition = gameObject->GetComponent<Transform>()->relativePosition;
				glm::vec3 startingRotation = gameObject->GetComponent<Transform>()->rotation;
				glm::vec3 startingScale = gameObject->GetComponent<Transform>()->scale;

				glm::vec3& currentPosition = gameObject->GetComponent<Transform>()->relativePosition;
				glm::vec3& currentRotation = gameObject->GetComponent<Transform>()->rotation;
				glm::vec3 rotationField = glm::degrees(currentRotation);
				glm::vec3& currentScale = gameObject->GetComponent<Transform>()->scale;


				ImGui::Text("Position: ");

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::DragFloat("X", &currentPosition.x);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::DragFloat("Y", &currentPosition.y, ImGuiSliderFlags_Logarithmic);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::DragFloat("Z", &currentPosition.z);


				ImGui::Text("Rotation: ");
				ImGui::PushID("Rotation");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("X", &rotationField.x);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("Y", &rotationField.y);

				ImGui::SameLine();
				ImGui::SetNextItemWidth(75);
				ImGui::InputFloat("Z", &rotationField.z);
				ImGui::PopID();
				currentRotation = glm::radians(rotationField);
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

				if (currentPosition != startingPosition || currentRotation != startingRotation || currentScale != startingScale) {
					gameObject->transform->UpdateChildrenTransform();
				}

				for (GameObject* child : gameObject->children) {
					ImGui::Text(child->name.c_str());
				}

				ImGui::PopID();
				ImGui::TreePop();
			}
		}
	};
}