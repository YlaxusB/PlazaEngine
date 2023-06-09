#pragma once
#include "Engine/GUI/Inspector.h"
#include "Engine/GUI/gizmo.h"
namespace Gui {
	class TransformInspector {
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
				ImGui::Text("World Rotation");
				ImGui::Text(std::to_string(gameObject->transform->worldRotation.x).c_str());
				ImGui::Text(std::to_string(gameObject->transform->worldRotation.y).c_str());
				ImGui::Text(std::to_string(gameObject->transform->worldRotation.z).c_str());
				ImGui::Text("Parent");
				ImGui::Text(gameObject->parent->name.c_str());



				ImGui::Text("Calculated relative to parent");
				
				//rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				//rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
				//rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
				glm::vec3 pos = gameObject->transform->relativePosition;
				glm::vec3 localPoint = gameObject->transform->relativePosition;//glm::vec3(glm::inverse(rotationMatrix) * glm::vec4(pos, 1.0f)) - gameObject->parent->transform->worldPosition;

				glm::vec3 desiredDirection = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));  // Example: Move in the positive X direction

				// Scale the direction vector by the desired distance
				float distance = 2.5f;  // Example: Move 2.5 units in the desired direction
				glm::vec3 displacement = distance * desiredDirection;

				// Move the object
				localPoint = moveTowards(gameObject->parent->transform->worldPosition, gameObject->parent->transform->worldRotation, gameObject->transform->relativePosition);//gameObject->parent->transform->worldPosition + displacement;

				glm::vec3 radiansRotation = glm::radians(gameObject->parent->transform->worldRotation);
				glm::mat4 rotationMatrix = glm::mat4(1.0f);
				rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

				glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(gameObject->transform->relativePosition, 1.0f));
				glm::vec3 finalWorldPoint = transformedPoint + gameObject->parent->transform->worldPosition;
				localPoint = finalWorldPoint;

				ImGui::Text(std::to_string(localPoint.x).c_str());
				ImGui::Text(std::to_string(localPoint.y).c_str());
				ImGui::Text(std::to_string(localPoint.z).c_str());
				ImGui::PopID();
				ImGui::TreePop();
			}
		}
	};
}