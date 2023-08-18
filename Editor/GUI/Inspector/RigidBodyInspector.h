#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Physics/RigidBody.h"
#include "Editor/GUI/Utils/DataVisualizer.h"
#include <functional>
#include <iostream>

RigidBody* currentBody = nullptr;
namespace Engine::Editor {
	void UpdateRigidBodyCallbackFloat(float value) {
		currentBody->UpdateRigidBody();
	}
	void UpdateRigidBodyCallbackVec3(glm::vec3 vec3) {
		currentBody->UpdateRigidBody();
	}

	static class RigidBodyInspector {
	public:
		RigidBodyInspector(RigidBody* rigidBody) {
			if (ImGui::TreeNodeEx("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::PushID("RigidBodyInspector");

				currentBody = rigidBody;
				physx::PxRigidBody& body = *rigidBody->m_rigidBody;
				bool dynamic = rigidBody->dynamic;
				glm::vec3& gravity = rigidBody->gravity;
				float& dynamicFriction = rigidBody->mDynamicFriction;
				float& staticFriction = rigidBody->mStaticFriction;
				float& restitution = rigidBody->mRestitution;
				float& density = rigidBody->density;

				//if (ImGui::DragFloat3("Gravity: ", &gravity.x, glm::min((gravity.x + gravity.y + gravity.z) / 300.0f, -0.01f), ImGuiInputTextFlags_CallbackEdit)) { UpdateRigidBodyCallbackVec3(gravity); };
				//ImGui::DragFloat("Dynamic Friction: ", &dynamicFriction, glm::min(dynamicFriction / 300.0f, -0.01f));
				Editor::Utils::DragFloat3("Gravity: ", gravity, 0.01f, &UpdateRigidBodyCallbackVec3);
				Editor::Utils::DragFloat("Dynamic Friction: ", dynamicFriction, 0.01f, &UpdateRigidBodyCallbackFloat, 0.0f);
				Editor::Utils::DragFloat("Static Friction: ", staticFriction, 0.01f, &UpdateRigidBodyCallbackFloat, 0.0f);
				Editor::Utils::DragFloat("Restitution: ", restitution, 0.01f, &UpdateRigidBodyCallbackFloat, 0.0f);
				Editor::Utils::DragFloat("Density: ", density, 0.01f, &UpdateRigidBodyCallbackFloat, 0.0001f);

				ImGui::PopID();
				ImGui::TreePop();
			}

		}
	};
}