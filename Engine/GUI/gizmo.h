#pragma once
#include "Engine/Vendor/imgui/ImGuizmo.h"
#include "Engine/Application/EditorCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "Engine/GUI/TransformOverlay.h"

//bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

namespace Gui {
	class Gizmo {
	public:
		Gizmo(GameObject* gameObject, Camera camera, AppSizes appSizes) {
			// Setup imguizmo
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

			// Get the object transform and camera matrices
			auto& transform = *gameObject->GetComponent<Transform>();
			glm::mat4 projection = camera.GetProjectionMatrix();//glm::perspective(glm::radians(camera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.1f, farplane);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 gizmoTransform = transform.GetTransform(gameObject->transform->worldPosition);

			ImGuizmo::OPERATION activeOperation = Gui::Overlay::activeOperation; // Operation is translate, rotate, scale
			ImGuizmo::MODE activeMode = Gui::Overlay::activeMode; // Mode is world or local

			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), activeOperation, activeMode, glm::value_ptr(gizmoTransform));


			if (ImGuizmo::IsUsing())
			{
				glm::vec3 position, rotation, scale;
				DecomposeTransform(gizmoTransform, position, rotation, scale); // The rotation is radians

				glm::vec3 radiansRotation = glm::radians(gameObject->parent->transform->worldRotation);
				glm::vec3 radiansRotation2 = glm::radians(gameObject->transform->worldRotation);
				glm::mat4 rotationMatrix = glm::mat4(1.0f);
				rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
				glm::vec3 localPoint = glm::vec3(glm::inverse(rotationMatrix) * glm::vec4(position, 1.0f));
				//rotationMatrix = glm::rotate(rotationMatrix, radiansRotation2.y, glm::vec3(0.0f, 1.0f, 0.0f));
				//rotationMatrix = glm::rotate(rotationMatrix, radiansRotation2.x, glm::vec3(1.0f, 0.0f, 0.0f));
				//rotationMatrix = glm::rotate(rotationMatrix, radiansRotation2.z, glm::vec3(0.0f, 0.0f, 1.0f));
				//transform.worldPosition = position;
				transform.relativePosition = localPoint;
				//transform.relativePosition = localPoint;

				radiansRotation = glm::radians(gameObject->transform->worldRotation);
				rotationMatrix = glm::mat4(1.0f);
				rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

				glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(localPoint, 1.0f));
				glm::vec3 finalWorldPoint = transformedPoint + gameObject->parent->transform->worldPosition;
				//transform.worldPosition = finalWorldPoint;

				//transform.relativePosition = (transform.worldPosition - gameObject->parent->transform->worldPosition) / gameObject->parent->transform->worldScale;

				glm::vec3 deltaRotation = glm::degrees(rotation) - transform.rotation;
				transform.rotation += deltaRotation;
				transform.scale = scale;

				gameObject->parent->transform->UpdateChildrenTransform();
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
