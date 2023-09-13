#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "Engine/Components/Physics/Collider.h"
#include "Editor/GUI/Utils/DataVisualizer.h"
#include <functional>
#include <iostream>

#include "Engine/Core/Physics.h"
namespace Plaza::Editor {


	static class ColliderInspector {
	public:
		ColliderInspector(Collider* collider) {
			ImGui::SetNextItemOpen(true);
			ImVec2 oldCursorPos = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 150.0f, ImGui::GetCursorPosY()));
			if (ImGui::Button("Remove Component")) {
				collider->GetGameObject()->RemoveComponent<Collider>();
			}
			ImGui::SameLine();
			ImGui::SetCursorPos(oldCursorPos);
			bool header = ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen);
			if (header) {
				ImGui::PushID("ColliderInspector");

				// Button for opening collider popup
				if (ImGui::Button("Add Shape"))
				{
					ImGui::OpenPopup("AddShapeContexMenu"); // Open the popup when the button is clicked
				}

				for (ColliderShape* shape : collider->mShapes) {
					ImGui::Text("Shape");
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
						collider->AddShape(new ColliderShape(shape, ColliderShapeEnum::BOX, 0));
					}
					if (ImGui::MenuItem("Sphere"))
					{
						physx::PxSphereGeometry geometry(1.0f);
						physx::PxShape* shape = Physics::m_physics->createShape(geometry, *defaultMaterial);
						collider->AddShape(new ColliderShape(shape, ColliderShapeEnum::SPHERE, 0));
					}
					if (ImGui::MenuItem("Mesh"))
					{
						collider->AddMeshShape(new Mesh(*Application->activeScene->meshRendererComponents.at(collider->uuid).mesh));
					}

					ImGui::EndPopup();
				}

				//for (physx::PxShape* shape : collider->mShapes) {
				//	ImGui::Text(shape->getName());


				//}

				ImGui::PopID();
			}
			// Button outside the header
			if (ImGui::Button("Button Outside Header")) {
				std::cout << "Button outside header clicked" << std::endl;
			}
		}
	};
}