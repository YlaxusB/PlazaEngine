#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
//#include "Editor/GUI/Inspector.h"
//#include "Editor/GUI/gizmo.h"
#include "Editor/GUI/Utils/DataVisualizer.h"
#include "Editor/GUI/Utils/Utils.h"

#include <math.h>
//#include "Editor/GUI/Utils/DataVisualizer.h"
namespace Plaza::Editor {
	class Gui::TransformInspector {
	public:
		glm::quat eulerToQuaternion(float pitch, float yaw, float roll) {
			glm::quat quaternion;
			quaternion = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll)));
			return quaternion;
		}
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


		TransformInspector(Scene* scene, Entity* entity) {
			TransformComponent* transform = scene->GetComponent<TransformComponent>(entity->uuid);
			if (Utils::ComponentInspectorHeader(transform, "Transform")) {
				glm::vec3 startingPosition = transform->relativePosition;
				glm::quat startingRotation = transform->rotation;
				glm::vec3 startingScale = transform->scale;

				glm::vec3& currentPosition = transform->relativePosition;
				glm::quat& currentRotation = transform->rotation;
				glm::vec3 rotationField = glm::degrees(transform->rotationEuler);//glm::degrees(glm::eulerAngles(currentRotation));
				glm::vec3& currentScale = transform->scale;

				if (glm::quat(transform->rotationEuler) != currentRotation)
					rotationField = glm::degrees(glm::eulerAngles(transform->rotation));

				if (Utils::DragFloat3("Position: ", currentPosition, 0.1f)) {
					//ImGuiSliderFlags_Logarithmic
					transform->UpdateSelfAndChildrenTransform();
				}

				ImGui::PushID("Rotation");
				if (Utils::DragFloat3("Rotation: ", rotationField, 0.1f)) {
					//ImGuiSliderFlags_Logarithmic
					glm::vec3 radians = glm::radians(rotationField);
					transform->rotationEuler = radians;
					transform->rotation = glm::quat(radians);
					//currentRotation *= glm::quat(glm::inverse(currentRotation) * glm::quat(radians));//glm::quat(glm::vec3(fmod(radians.x, 360.0f), fmod(radians.y, 360.0f), fmod(radians.z, 360.0f)));
					transform->UpdateSelfAndChildrenTransform();
				}
				ImGui::PopID();

				ImGui::PushID("Scale");
				if (Utils::DragFloat3("Scale: ", currentScale, 0.1f)) {
					//ImGuiSliderFlags_Logarithmic
					transform->UpdateSelfAndChildrenTransform();
				}

				if (currentPosition != startingPosition || currentRotation != startingRotation || currentScale != startingScale) {

//					entity->GetComponent<Transform>()->UpdateSelfAndChildrenTransform();
					if (scene->HasComponent<RigidBody>(entity->uuid)) {
						//RigidBody* rigidBody = &Scene::GetActiveScene()->rigidBodyComponents.at(entity->uuid);
						//rigidBody->UpdateGlobalPose();
					}
				}

				for (uint64_t child : entity->childrenUuid) {
					ImGui::Text(Scene::GetActiveScene()->entities[child].name.c_str());
				}


				ImGui::PopID();
			}
		}
	};
}