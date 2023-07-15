#include "Engine/Core/PreCompiledHeaders.h"
#include "gizmo.h"
#include "Engine/GUI/guiMain.h"
#include "Engine/GUI/TransformOverlay.h"

namespace Engine::Editor {
	void Gizmo::Draw(GameObject* gameObject, Camera camera) {
		
		ApplicationSizes& appSizes = *Application->appSizes;
		// Setup imguizmo
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

		// Get the object transform and camera matrices
		auto& parentTransform = *gameObject->parent->GetComponent<Transform>();
		auto& transform = *gameObject->GetComponent<Transform>();

		glm::mat4 projection = camera.GetProjectionMatrix();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 gizmoTransform = transform.GetTransform(gameObject->transform->worldPosition);
		ImGuizmo::OPERATION activeOperation = Overlay::activeOperation; // Operation is translate, rotate, scale
		ImGuizmo::MODE activeMode = Overlay::activeMode; // Mode is world or local

		ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), activeOperation, activeMode, glm::value_ptr(gizmoTransform));

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 position, rotation, scale;
			DecomposeTransform(gizmoTransform, position, rotation, scale);

			// --- Rotation
			glm::mat4 updatedTransform = glm::translate(glm::mat4(1.0f), position)
				* glm::toMat4(glm::inverse(glm::quat(parentTransform.worldRotation)))
				* glm::toMat4(glm::quat(rotation))
				* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

			glm::vec3 updatedPosition, updatedRotation, updatedScale;
			DecomposeTransform(updatedTransform, updatedPosition, updatedRotation, updatedScale); // The rotation is radians
			rotation = updatedRotation;

			// Adding the deltaRotation avoid the gimbal lock
			glm::vec3 deltaRotation = rotation - transform.rotation;
			transform.rotation += deltaRotation;

			// --- Position
			// Get the position in the world and transform it to be a localPosition in relation to the parent
			position = position - parentTransform.worldPosition;
			position = position / parentTransform.worldScale;

			glm::vec3 parentWorldRotation = parentTransform.worldRotation;
			glm::mat4 rotationMatrix = glm::mat4(1.0f);
			rotationMatrix = glm::rotate(rotationMatrix, -parentWorldRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
			rotationMatrix = glm::rotate(rotationMatrix, -parentWorldRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
			rotationMatrix = glm::rotate(rotationMatrix, -parentWorldRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

			glm::vec3 localPoint = glm::vec3(rotationMatrix * glm::vec4(position, 1.0f));
			transform.relativePosition = localPoint;

			// --- Scale
			transform.scale = scale / parentTransform.worldScale;

			transform.UpdateChildrenTransform();
		}
		
	}


	bool Gizmo::DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
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
}