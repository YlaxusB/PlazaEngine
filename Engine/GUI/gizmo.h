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
glm::vec3 GetLocalRotation(const glm::vec3& worldRotation, const glm::vec3& parentRotation);

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


			ImGuizmo::GetStyle().RotationLineThickness = 6.0f;

			// Get the object transform and camera matrices
			auto& transform = *gameObject->GetComponent<Transform>();
			glm::mat4 projection = camera.GetProjectionMatrix();//glm::perspective(glm::radians(camera.Zoom), (float)(appSizes.sceneSize.x / appSizes.sceneSize.y), 0.1f, farplane);
			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 gizmoTransform = transform.GetTransform(gameObject->transform->worldPosition);
			glm::mat4 originalMatrix = transform.GetTransform(gameObject->transform->worldPosition);

			ImGuizmo::OPERATION activeOperation = Gui::Overlay::activeOperation; // Operation is translate, rotate, scale
			ImGuizmo::MODE activeMode = Gui::Overlay::activeMode; // Mode is world or local

			glm::mat4 deltaMatrix;

			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), activeOperation, activeMode, glm::value_ptr(gizmoTransform));

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 position, rotation, scale;
				DecomposeTransform(gizmoTransform, position, rotation, scale); // The rotation is radians

				position = position - gameObject->parent->transform->worldPosition;// / gameObject->parent->transform->worldScale;

				position = position / gameObject->parent->transform->worldScale;
				glm::mat4 updatedTransform = glm::translate(glm::mat4(1.0f), position)
					* glm::toMat4(glm::inverse(glm::quat(gameObject->parent->transform->worldRotation)))
					* glm::toMat4(glm::quat(rotation))
					* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

				glm::vec3 position2, rotation2, scale2;
				DecomposeTransform(updatedTransform, position2, rotation2, scale2); // The rotation is radians
				transform.rotation += (rotation2 - transform.rotation);
				//position = position2;//   / gameObject->parent->transform->worldScale;
				rotation = rotation2;

				//transform.rotation += deltaRotation3;


				
				//transform.worldRotation += deltaRotation;// - gameObject->parent->transform->worldRotation;//deltaRotation;
				//transform.worldRotation = transform.rotation + transform.gameObject->parent->transform->worldRotation;
				glm::vec3 worldPosition;  // The world position you want to transform to local space

				// Calculate the relative position by subtracting the parent's world position
				glm::vec3 relativePosition = position;

				// Calculate the inverse rotation matrix based on the euler angles
				glm::vec3 radiansRotation = gameObject->parent->transform->worldRotation;
				glm::mat4 rotationMatrix = glm::mat4(1.0f);
				rotationMatrix = glm::rotate(rotationMatrix, -radiansRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
				rotationMatrix = glm::rotate(rotationMatrix, -radiansRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));

				rotationMatrix = glm::rotate(rotationMatrix, -radiansRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

				// Transform the relative position using the inverse rotation matrix
				glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(relativePosition, 1.0f));

				// The resulting transformedPoint is the position in local space
				glm::vec3 localPoint = transformedPoint;
				position = localPoint;
				std::cout << "x";
				std::cout << (position.x);
				std::cout << "y";
				std::cout << (position.y);
				std::cout << "z";
				std::cout << (position.z) << std::endl;

				// THIS WAS MOVING THE OBJECT \/ -------------------------------------------------------------------------------------------
				transform.relativePosition = localPoint;

				// may be useful
				//transform.relativePosition = (transform.worldPosition - gameObject->parent->transform->worldPosition) / gameObject->parent->transform->worldScale;


				transform.scale = scale / gameObject->parent->transform->worldScale;

				gameObject->parent->transform->UpdateChildrenTransform();
			}
		}


		glm::vec3 GetLocalRotation(const glm::vec3& worldRotation, const glm::vec3& parentRotation)
		{
			// Convert world rotation to quaternion representation
			glm::quat worldRotationQuat = glm::quat(worldRotation);

			// Convert parent rotation to quaternion representation
			glm::quat parentRotationQuat = glm::quat(parentRotation);

			// Calculate the inverse of the parent's rotation quaternion
			glm::quat parentRotationInverse = glm::inverse(parentRotationQuat);

			// Calculate the local rotation quaternion by multiplying the world rotation by the inverse of the parent rotation
			glm::quat localRotationQuat = worldRotationQuat * parentRotationInverse;

			// Convert the local rotation quaternion back to euler angles (vec3 representation)
			glm::vec3 localRotation = glm::eulerAngles(localRotationQuat);

			return localRotation;
		}
	private:

		float GetRotationAngle(const glm::mat4& matrixA, const glm::mat4& matrixB)
		{
			glm::quat rotationA = glm::normalize(glm::quat(matrixA));
			glm::quat rotationB = glm::normalize(glm::quat(matrixB));

			// Calculate the rotation difference between the two quaternions
			glm::quat rotationDifference = glm::inverse(rotationA) * rotationB;

			// Calculate the angle in radians from the quaternion
			float angle = 2.0f * acos(rotationDifference.w);

			return angle;
		}

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
