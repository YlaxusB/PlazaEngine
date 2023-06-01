#pragma once
#include "Engine/Vendor/imgui/ImGuizmo.h"
#include "Engine/Application/EditorCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

//bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

namespace Gui {
	class Gizmo {
	public:
		Gizmo(GameObject* gameObject, Camera camera, AppSizes appSizes) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);
			//ImGuizmo::SetRect(appSizes.hierarchySize.x, 0, appSizes.appSize.x, appSizes.appSize.y);
			// Create a view and projection matrix for the camera.
			//glm::mat4 projection = glm::mat4(1.0f);//glm::perspective(glm::radians(camera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.3f, 100.0f);
			//glm::mat4 view = camera.GetViewMatrix();
			auto& tc = *gameObject->GetComponent<Transform>();
			glm::mat4 projection = camera.GetProjectionMatrix();//glm::perspective(glm::radians(camera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.1f, farplane);
			glm::mat4 view = camera.GetViewMatrix();
			// Create a matrix that represents the current state of the object.
			//glm::mat4& objectMatrix = gameObject->GetComponent<Transform>()->GetTransform();//glm::translate(objectMatrix, glm::vec3(0, 0, 0));


			// Entity transform

			glm::mat4 gizmoTransform = glm::mat4(1.0f);
			gizmoTransform = glm::scale(gizmoTransform, tc.scale);
			gizmoTransform = glm::translate(gizmoTransform, tc.position);
			gizmoTransform = glm::rotate(gizmoTransform, glm::radians(tc.rotation.z), glm::vec3(0, 0, 1)); // Rotate around the Z-axis
			gizmoTransform = glm::rotate(gizmoTransform, glm::radians(tc.rotation.y), glm::vec3(0, 1, 0)); // Rotate around the Y-axis
			gizmoTransform = glm::rotate(gizmoTransform, glm::radians(tc.rotation.x), glm::vec3(1, 0, 0)); // Rotate around the X-axis


			//ImGuizmo::RecomposeMatrixFromComponents
			glm::mat4 transform = tc.GetTransform();
			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transform));
			if (ImGuizmo::IsUsing())
			{
				glm::vec3 position, rotation, scale;
				DecomposeTransform(transform, position, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.rotation;
				tc.position = position;
				std::cout << (rotation - tc.rotation).x << std::endl;
				//tc.rotation = rotation - tc.rotation;
				//tc.scale = scale;
			}
		}
	private:

		bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
		{
			// From glm::decompose in matrix_decompose.inl

			using namespace glm;
			using T = float;

			mat4 LocalMatrix(transform);

			// Normalize the matrix.
			if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
				return false;

			// First, isolate perspective.  This is the messiest.
			if (
				epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
			{
				// Clear the perspective partition
				LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
				LocalMatrix[3][3] = static_cast<T>(1);
			}

			// Next take care of translation (easy).
			translation = vec3(LocalMatrix[3]);
			LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

			vec3 Row[3], Pdum3;

			// Now get scale and shear.
			for (length_t i = 0; i < 3; ++i)
				for (length_t j = 0; j < 3; ++j)
					Row[i][j] = LocalMatrix[i][j];

			// Compute X scale factor and normalize first row.
			scale.x = length(Row[0]);
			Row[0] = detail::scale(Row[0], static_cast<T>(1));
			scale.y = length(Row[1]);
			Row[1] = detail::scale(Row[1], static_cast<T>(1));
			scale.z = length(Row[2]);
			Row[2] = detail::scale(Row[2], static_cast<T>(1));

			// At this point, the matrix (in rows[]) is orthonormal.
			// Check for a coordinate system flip.  If the determinant
			// is -1, then negate the matrix and the scaling factors.
#if 0
			Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
			if (dot(Row[0], Pdum3) < 0)
			{
				for (length_t i = 0; i < 3; i++)
				{
					scale[i] *= static_cast<T>(-1);
					Row[i] *= static_cast<T>(-1);
				}
			}
#endif

			rotation.y = asin(-Row[0][2]);
			if (cos(rotation.y) != 0) {
				rotation.x = atan2(Row[1][2], Row[2][2]);
				rotation.z = atan2(Row[0][1], Row[0][0]);
			}
			else {
				rotation.x = atan2(-Row[2][0], Row[1][1]);
				rotation.z = 0;
			}


			return true;
		}

	};
}
