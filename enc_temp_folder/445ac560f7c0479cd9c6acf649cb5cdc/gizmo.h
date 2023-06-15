#pragma once
#include "Engine/Vendor/imgui/ImGuizmo.h"
#include "Engine/Application/EditorCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/euler_angles.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


#include <float.h>
#include <glm/gtx/euler_angles.hpp>

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
				//gizmoTransform *= -gameObject->parent->transform->worldRotation;

				/*
				//gizmoTransform = glm::rotate(gizmoTransform, -gameObject->parent->transform->worldRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
				//gizmoTransform = glm::rotate(gizmoTransform, -gameObject->parent->transform->worldRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				//gizmoTransform = glm::rotate(gizmoTransform, -gameObject->parent->transform->worldRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

				//gizmoTransform = glm::rotate(gizmoTransform, -gameObject->parent->transform->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
				//gizmoTransform = glm::rotate(gizmoTransform, -gameObject->parent->transform->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				//gizmoTransform = glm::rotate(gizmoTransform, -gameObject->parent->transform->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
				*/



				//glm::mat4 gizmoMatrix = glm::translate(glm::mat4(1.0f), gameObject->transform->worldPosition)
				//	* glm::toMat4(glm::quat(gameObject->parent->transform->worldRotation))
				//	* glm::toMat4(glm::quat(gameObject->transform->rotation))
				//	* glm::scale(glm::mat4(1.0f), gameObject->transform->worldScale);

				//glm::vec3 a = glm::eulerAngles(glm::quat_cast(gizmoMatrix));

				// Get the rotation quaternion from the gameObject
				glm::quat gameObjectRotation = glm::quat(gameObject->transform->worldRotation);

				// Apply the inverse of the parent's world rotation to get the local rotation
				glm::quat localRotation = glm::inverse(glm::quat(glm::eulerAngles(glm::quat_cast(gizmoTransform)))) * gameObjectRotation;

				// Update the rotation component of the gizmoTransform matrix
				glm::mat4 correctTransform = gizmoTransform
					* glm::toMat4(localRotation)
					* glm::scale(glm::mat4(1.0f), gameObject->transform->worldScale);

				glm::vec3 position, rot, scale;
				DecomposeTransform(gizmoTransform, position, rot, scale); // The rotation is radians
				rot = glm::eulerAngles(glm::quat_cast(gizmoTransform));
				rot -= gameObject->transform->worldRotation;
				rot += gameObject->transform->rotation;
				std::cout << "x";
				std::cout << glm::degrees(rot.x);
				std::cout << "y";
				std::cout << glm::degrees(rot.y);
				std::cout << "z";
				std::cout << glm::degrees(rot.z) << std::endl;
				/*
				ImGuizmo::DecomposeMatrixToComponents(
					glm::value_ptr(gizmoTransform), // Pass the pointer to the matrix data
					glm::value_ptr(position),      // Output translation
					glm::value_ptr(rot),         // Output rotation
					glm::value_ptr(scale)             // Output scale
				);
				*/
				//glm::vec3 rotation =;//glm::vec3(rot.y, rot.z, rot.x);;//glm::eulerAngles(glm::quat_cast(gizmoTransform));
				glm::vec3 rotation = glm::vec3(rot.x, rot.y, rot.z);
				//rotation = glm::eulerAngles(glm::quat_cast(gizmoTransform));
				//rotation -= transform.worldRotation;

				//rotation = glm::radians(rotation);
				glm::vec3 deltaRotation = rotation - transform.rotation;

				transform.rotation += deltaRotation;// - gameObject->parent->transform->worldRotation;//deltaRotation;
				//transform.worldRotation = transform.rotation + transform.gameObject->parent->transform->worldRotation;
				glm::vec3 worldPosition;  // The world position you want to transform to local space

				// Calculate the relative position by subtracting the parent's world position
				glm::vec3 relativePosition = position - gameObject->parent->transform->worldPosition;

				// Calculate the inverse rotation matrix based on the euler angles
				glm::vec3 radiansRotation = gameObject->parent->transform->worldRotation;
				glm::mat4 rotationMatrix = glm::mat4(1.0f);
				rotationMatrix = glm::rotate(rotationMatrix, -radiansRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
				rotationMatrix = glm::rotate(rotationMatrix, -radiansRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, -radiansRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));

				// Transform the relative position using the inverse rotation matrix
				glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(relativePosition, 1.0f));

				// The resulting transformedPoint is the position in local space
				glm::vec3 localPoint = transformedPoint;


				transform.relativePosition = localPoint;

				//transform.relativePosition = (transform.worldPosition - gameObject->parent->transform->worldPosition) / gameObject->parent->transform->worldScale;


				//transform.scale = scale;

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
