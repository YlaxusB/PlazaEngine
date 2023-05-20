#pragma once
#include "Engine/Vendor/imgui/ImGuizmo.h"
#include "Engine/Application/EditorCamera.h"
#include <glm/glm.hpp>
namespace Gui {
	class Gizmo {
	public:
		Gizmo(GameObject* gameObject, Camera camera, AppSizes appSizes) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(0, 0, 0 - 0, 0 - 0);

			// Editor camera
			glm::mat4 cameraProjection = glm::perspective(glm::radians(camera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.1f, 100.0f);
			glm::mat4 cameraView = camera.GetViewMatrix();

			// Entity transform
			//auto& tc = gameObject->GetComponent<Transform>();
			//glm::mat4 transform = tc.GetTransform();
			glm::mat4 transform = glm::mat4(0.0f);
			int m_GizmoType = 1;
			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				//Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = glm::vec3(0.0f); // -tc.Rotation;
				//tc.Translation = translation;
				//tc.Rotation += deltaRotation;
				//tc.Scale = scale;
			}
		}
	};
}