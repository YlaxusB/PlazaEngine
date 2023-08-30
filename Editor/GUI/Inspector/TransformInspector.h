#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
//#include "Editor/GUI/Inspector.h"
//#include "Editor/GUI/gizmo.h"
#include "Editor/GUI/Utils/DataVisualizer.h"
//#include "Editor/GUI/Utils/DataVisualizer.h"
namespace Plaza::Editor {
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


		TransformInspector(Entity* entity) {
			if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
				glm::vec3 startingPosition = entity->GetComponent<Transform>()->relativePosition;
				glm::vec3 startingRotation = entity->GetComponent<Transform>()->rotation;
				glm::vec3 startingScale = entity->GetComponent<Transform>()->scale;

				glm::vec3& currentPosition = entity->GetComponent<Transform>()->relativePosition;
				glm::vec3& currentRotation = entity->GetComponent<Transform>()->rotation;
				glm::vec3 rotationField = glm::degrees(currentRotation);
				glm::vec3& currentScale = entity->GetComponent<Transform>()->scale;

				if (Utils::DragFloat3("Position: ", currentPosition, 0.1f)) {
					//ImGuiSliderFlags_Logarithmic
					entity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
				}

				ImGui::PushID("Rotation");
				if (Utils::DragFloat3("Rotation: ", rotationField, 0.1f)) {
					//ImGuiSliderFlags_Logarithmic
					currentRotation = glm::radians(rotationField);
					entity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
				}
				ImGui::PopID();

				ImGui::PushID("Scale");
				if (Utils::DragFloat3("Scale: ", currentScale, 0.1f)) {
					//ImGuiSliderFlags_Logarithmic
					entity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
				}

				if (currentPosition != startingPosition || currentRotation != startingRotation || currentScale != startingScale) {

//					entity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
					if (Application->activeScene->rigidBodyComponents.find(entity->uuid) != Application->activeScene->rigidBodyComponents.end()) {
						//RigidBody* rigidBody = &Application->activeScene->rigidBodyComponents.at(entity->uuid);
						//rigidBody->UpdateGlobalPose();
					}
				}

				for (uint64_t child : entity->childrenUuid) {
					ImGui::Text(Application->activeScene->entities[child].name.c_str());
				}



				ImGui::PopID();
				ImGui::TreePop();
			}
		}
	};
}