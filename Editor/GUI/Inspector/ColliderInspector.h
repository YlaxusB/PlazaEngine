#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "Engine/Components/Physics/Collider.h"
#include "Editor/GUI/Utils/DataVisualizer.h"
#include <functional>
#include <iostream>

#include "Engine/Core/Physics.h"
namespace Engine::Editor {


	static class ColliderInspector {
	public:
		ColliderInspector(Collider* collider) {
			if (ImGui::TreeNodeEx("Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::PushID("ColliderInspector");

				// Button for opening collider popup
				if (ImGui::Button("Add Shape"))
				{
					ImGui::OpenPopup("AddShapeContexMenu"); // Open the popup when the button is clicked
				}

				// Check if the context menu is open
				if (ImGui::BeginPopupContextWindow("AddShapeContexMenu"))
				{
					if (ImGui::IsMouseReleased(1)) // Check if right mouse button was released
					{
						ImGui::CloseCurrentPopup(); // Close the context menu on right-click
					}

					Transform& transform = *Application->activeScene->entities.at(collider->uuid).GetComponent<Transform>();
					// Create a dynamic rigid body
					glm::quat quaternion = transform.GetWorldQuaternion();
					physx::PxQuat pxQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

					physx::PxTransform* pxTransform = new physx::PxTransform(
						transform.worldPosition.x, transform.worldPosition.y, transform.worldPosition.z,
						pxQuaternion);

					physx::PxMaterial* defaultMaterial = Physics::m_physics->createMaterial(0.0f, 0.0f, 0.5f);
					if (ImGui::MenuItem("Box"))
					{
						physx::PxBoxGeometry geometry(transform.scale.x / 2.1, transform.scale.y / 2.1, transform.scale.z / 2.1);
						physx::PxShape* shape = Physics::m_physics->createShape(geometry, *defaultMaterial);
						collider->AddShape(shape);
					}
					if (ImGui::MenuItem("Sphere"))
					{
						physx::PxSphereGeometry geometry(1.0f);
						physx::PxShape* shape = Physics::m_physics->createShape(geometry, *defaultMaterial);
						collider->AddShape(shape);
					}


					ImGui::EndPopup();
				}

				//for (physx::PxShape* shape : collider->mShapes) {
				//	ImGui::Text(shape->getName());


				//}

				ImGui::PopID();
				ImGui::TreePop();
			}

		}
	};
}