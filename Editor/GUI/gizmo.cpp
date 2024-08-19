#include "Engine/Core/PreCompiledHeaders.h"
#include "gizmo.h"
#include "Editor/GUI/guiMain.h"
#include "Editor/GUI/TransformOverlay.h"

namespace Plaza::Editor {
	glm::vec3 WorldToLocal(const glm::mat4& worldToLocalMatrix, const glm::vec3& worldPoint) {
		// Calculate the inverse of the world-to-local transformation matrix
		glm::mat4 localToWorldMatrix = glm::inverse(worldToLocalMatrix);

		// Transform the world point to local space
		glm::vec4 localPoint4 = localToWorldMatrix * glm::vec4(worldPoint, 1.0f);

		// Convert the resulting vector to a vec3 and return it
		return glm::vec3(localPoint4) / localPoint4.w;
	}

	glm::vec3 WorldToLocalEulerAngles(const glm::vec3& worldEulerAngles, const glm::vec3& localEulerAngles) {
		// Calculate the relative Euler angles between world and local rotations
		glm::vec3 relativeEulerAngles = worldEulerAngles - localEulerAngles;

		// Calculate the local Euler angles by applying the relative angles to the world angles
		glm::vec3 localEulerAnglesResult = worldEulerAngles - relativeEulerAngles;

		return localEulerAnglesResult;
	}

	void Gizmo::Draw(Entity* entity, Camera camera) {

		ApplicationSizes& appSizes = *Application->appSizes;
		// Setup imguizmo
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::GetStyle().HatchedAxisLineThickness = 8.0f;
		ImGuizmo::GetStyle().RotationLineThickness = 6.0f;
		ImGuizmo::GetStyle().RotationOuterLineThickness = 4.0f;
		ImGuizmo::GetStyle().ScaleLineCircleSize = 8.0f;
		ImGuizmo::GetStyle().ScaleLineThickness = 6.0f;
		ImGuizmo::GetStyle().TranslationLineArrowSize = 10.0f;
		ImGuizmo::GetStyle().TranslationLineThickness = 6.0f;
		ImGuizmo::SetDrawlist();
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImGuizmo::SetRect(appSizes.sceneImageStart.x, appSizes.sceneImageStart.y, appSizes.sceneSize.x, appSizes.sceneSize.y);

		// Get the object transform and camera matrices
		Transform& a = *entity->GetComponent<Transform>();
		Transform& b = *entity->GetComponent<Transform>();
		auto& parentTransform = *Application->activeScene->entities[entity->parentUuid].GetComponent<Transform>();
		auto& transform = *entity->GetComponent<Transform>();

		glm::mat4 projection = camera.GetProjectionMatrix();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 gizmoTransform = transform.modelMatrix;//transform.GetTransform(entity->GetComponent<Transform>()->worldPosition);
		ImGuizmo::OPERATION activeOperation = Overlay::activeOperation; // Operation is translate, rotate, scale
		ImGuizmo::MODE activeMode = Overlay::activeMode; // Mode is world or local

		ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), activeOperation, activeMode, glm::value_ptr(gizmoTransform));

		RigidBody* rigidBody = entity->GetComponent<RigidBody>();
		Collider* collider = entity->GetComponent<Collider>();
		if (rigidBody && rigidBody->mRigidActor != nullptr) {
			rigidBody->mRigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
			rigidBody->canUpdate = false;
		}

		if (collider && !collider->mDynamic && collider->mStaticPxRigidBody) {
			collider->mRigidActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);
		}

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 position, rotation, scale;
			DecomposeTransform(gizmoTransform, position, rotation, scale);

			// --- Rotation
			glm::mat4 updatedTransform = glm::inverse(parentTransform.GetTransform()) * glm::toMat4(glm::quat(rotation));
			/*
			glm::translate(glm::mat4(1.0f), position)
			* glm::toMat4(glm::inverse(glm::quat(parentTransform.GetWorldQuaternion())))
			* glm::toMat4(glm::quat(rotation))
			* glm::scale(glm::mat4(1.0f), parentTransform.scale);
			*/
			glm::vec3 parentPosition, parentRotation, parentScale;
			DecomposeTransform(parentTransform.modelMatrix, parentPosition, parentRotation, parentScale); // The rotation is radians

			glm::vec3 updatedPosition, updatedRotation, updatedScale;
			DecomposeTransform(updatedTransform, updatedPosition, updatedRotation, updatedScale); // The rotation is radians

			glm::mat4 rotationMatrix = glm::inverse(glm::mat4_cast(parentTransform.GetWorldQuaternion()));
			//	rotation = glm::eulerAngles(glm::quat(glm::inverse(transform.modelMatrix) * (glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z))));
			rotation = updatedRotation;
			//rotation = -parentRotation * rotation;
			// Adding the deltaRotation avoid the gimbal lock
			glm::vec3 eulerRotation = rotation;
			glm::quat quaternion = glm::quat_cast(glm::mat3(glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))));
			glm::quat deltaRotation = quaternion - transform.rotation;
			transform.rotation = rotation;
			//std::cout << "X: " << glm::degrees(rotation.x) << std::endl;
			//std::cout << "Y: " << glm::degrees(rotation.y) << std::endl;
			//std::cout << "Z: " << glm::degrees(rotation.z) << std::endl;
			//transform.rotation += rotation;

			// --- Position
			// Get the position in the world and transform it to be a localPosition in relation to the parent
			//position = position - transform.relativePosition;
			//position = position / parentTransform.worldScale;

			glm::vec3 parentWorldRotation = parentRotation;
			rotationMatrix =
				glm::mat4(1.0f);
			rotationMatrix = glm::rotate(rotationMatrix, -parentWorldRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
			rotationMatrix = glm::rotate(rotationMatrix, -parentWorldRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
			rotationMatrix = glm::rotate(rotationMatrix, -parentWorldRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));


			glm::vec3 localPoint = glm::vec3(rotationMatrix * glm::vec4(position - parentPosition, 1.0f));
			transform.relativePosition = localPoint / parentScale;//localPoint;

			// --- Scale
			if (activeOperation == ImGuizmo::SCALE)
				transform.scale = scale / parentScale;//transform.GetWorldScale();//parentTransform.GetWorldScale();// = updatedScale;
			transform.UpdateSelfAndChildrenTransform();



			// Update Rigid Body Position
			//if (ImGuizmo::IsUsing()) {
			//	glm::quat quaternion = transform.GetWorldQuaternion();
			//	physx::PxQuat pxQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

			//	physx::PxTransform* pxTransform = new physx::PxTransform(
			//		transform.worldPosition.x, transform.worldPosition.y, transform.worldPosition.z,
			//		pxQuaternion);

			//	// Apply scaling to the existing pxTransform
			//	if (rigidBody && rigidBody->mRigidActor)
			//		rigidBody->mRigidActor->setGlobalPose(*pxTransform);
			//	else if (collider && !collider->mDynamic && collider->mStaticPxRigidBody)
			//		collider->mStaticPxRigidBody->setGlobalPose(*pxTransform);


			//}
			//transform.SetRelativeScale(transform.scale);
			if (collider) {
				collider->UpdateAllShapesScale();

				//if (collider->mDynamic) {
				//	collider->mRigidActor->is<physx::PxRigidDynamic>()->setLinearVelocity(physx::PxVec3(0.0f));
				//	collider->mRigidActor->is<physx::PxRigidDynamic>()->setAngularVelocity(physx::PxVec3(0.0f));
				//}
			}
		}

		if (rigidBody && rigidBody->mRigidActor && !ImGuizmo::IsUsing()) {
			rigidBody->mRigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
			rigidBody->canUpdate = true;
		}

		if (collider && !collider->mDynamic && collider->mStaticPxRigidBody) {
			collider->mRigidActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
		}

		if (Application->runningScene && !ImGuizmo::IsUsing()) {
			//collider->UpdateShapeScale(transform.worldScale);
			//transform.SetRelativeScale(transform.worldScale);
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