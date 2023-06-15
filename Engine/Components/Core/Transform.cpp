#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/euler_angles.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


#include "Engine/Components/Core/Transform.h"
//#include "Engine/GUI/gizmo.h"
//#include "Engine/Components/Core/GameObject.h"

glm::vec3 ConvertToRadians(const glm::vec3& anglesInDegrees);
Transform::Transform() {};

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

// Get transform
glm::mat4 Transform::GetTransform(glm::vec3 position)
{	
	if (this->gameObject->parent == nullptr) {
		return glm::mat4(0.0f);
	}
	glm::mat4 rot = glm::toMat4(glm::quat(rotation));

	glm::mat4 gizmoMatrix = glm::translate(glm::mat4(1.0f), this->worldPosition)
		* glm::toMat4(glm::quat(gameObject->transform->worldRotation))
		* glm::scale(glm::mat4(1.0f), gameObject->transform->worldScale);
	return gizmoMatrix;

	/*
		glm::mat4 gizmoMatrix = glm::translate(glm::mat4(1.0f), this->worldPosition)
		* glm::toMat4(glm::quat(gameObject->parent->transform->worldRotation))
		* glm::toMat4(glm::quat(gameObject->transform->rotation))
		* glm::scale(glm::mat4(1.0f), gameObject->transform->worldScale);
	return gizmoMatrix;
	*/
}

glm::mat4 Transform::GetTransform() {
	return GetTransform(this->worldPosition);
}

glm::vec3 ConvertToRadians(const glm::vec3& anglesInDegrees)
{
	return glm::vec3(glm::radians(anglesInDegrees.x),
		glm::radians(anglesInDegrees.y),
		glm::radians(anglesInDegrees.z));
}

glm::mat4 CreateTransformationMatrix(const glm::vec3& position,
	const glm::vec3& rotation,
	const glm::vec3& scale)
{
	glm::mat4 matrix = glm::mat4(1.0f);

	matrix = glm::translate(matrix, position);
	matrix = glm::rotate(matrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::rotate(matrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	matrix = glm::scale(matrix, scale);

	return matrix;
}

glm::vec3 TransformToWorldSpace(const glm::vec3& localPosition,
	const glm::mat4& transformMatrix)
{
	glm::vec4 homogeneousPos(localPosition.x, localPosition.y, localPosition.z, 1.0f);
	glm::vec4 worldPos = transformMatrix * homogeneousPos;

	return glm::vec3(worldPos);
}

glm::vec3 TransformToLocalSpace(const glm::vec3& worldPosition,
	const glm::mat4& transformMatrix)
{
	glm::mat4 inverseMatrix = glm::inverse(transformMatrix);
	glm::vec4 homogeneousPos(worldPosition.x, worldPosition.y, worldPosition.z, 1.0f);
	glm::vec4 localPos = inverseMatrix * homogeneousPos;

	return glm::vec3(localPos);
}

glm::vec3 test(GameObject* child) {
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	rotationMatrix *= glm::toMat4(glm::quat(child->parent->transform->worldRotation));

	glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(child->transform->relativePosition, 1.0f));
	glm::vec3 finalWorldPoint = transformedPoint + child->parent->transform->worldPosition;

	return finalWorldPoint;
}

glm::vec3 newWorldRotation(GameObject* gameObject) {
	glm::mat4 gizmoMatrix = glm::translate(glm::mat4(1.0f), gameObject->transform->worldPosition)
		* glm::toMat4(glm::quat(gameObject->parent->transform->worldRotation))
		* glm::toMat4(glm::quat(gameObject->transform->rotation))
		* glm::scale(glm::mat4(1.0f), gameObject->transform->worldScale);

	glm::vec3 a = glm::eulerAngles(glm::quat_cast(gizmoMatrix));
	return a;

	//return gameObject->transform->rotation + gameObject->parent->transform->worldRotation;
}


void Transform::UpdateChildrenTransform(GameObject* gameObject) {
	if (gameObject->parent != nullptr) {
		gameObject->transform->worldScale = gameObject->transform->scale * gameObject->parent->transform->worldScale;
		gameObject->transform->worldPosition = (gameObject->transform->relativePosition * gameObject->parent->transform->worldScale + gameObject->parent->transform->worldPosition);
		gameObject->transform->worldRotation = newWorldRotation(gameObject);//gameObject->transform->rotation + gameObject->parent->transform->worldRotation;
		gameObject->transform->worldPosition = test(gameObject);



		glm::quat radiansRotation = gameObject->parent->transform->worldRotation;
		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(gameObject->transform->relativePosition, 1.0f));
		glm::vec3 finalWorldPoint = transformedPoint + gameObject->parent->transform->worldPosition;
		//gameObject->transform->worldPosition = test(gameObject);//finalWorldPoint;

	}

	for (GameObject* child : gameObject->children) {
		child->transform->worldScale = child->transform->scale * child->parent->transform->worldScale;
		child->transform->worldPosition = (child->transform->relativePosition * child->parent->transform->worldScale + child->parent->transform->worldPosition);
		child->transform->worldRotation = newWorldRotation(child);//child->transform->rotation + child->parent->transform->worldRotation;
		child->transform->worldPosition = test(child);


		/*
		glm::vec3 radiansRotation = glm::radians(child->transform->worldRotation);
		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(child->transform->relativePosition, 1.0f));
		glm::vec3 finalWorldPoint = transformedPoint + child->parent->transform->worldPosition;
		child->transform->worldPosition = finalWorldPoint;
		//glm::vec3 localPoint = glm::vec3(rotationMatrix * glm::vec4(translatedPoint, 1.0f));
		*/
		UpdateChildrenTransform(child);
	}

}



void Transform::UpdateChildrenTransform() {
	if (gameObject != nullptr) {
		UpdateChildrenTransform(gameObject);
	}
}


void Transform::UpdateChildrenScale(GameObject* gameObject) {
	/*
	if (gameObject->parent != nullptr) {
		gameObject->transform->worldScale = gameObject->transform->scale * gameObject->parent->transform->worldScale;
		gameObject->transform->worldPosition = (gameObject->transform->relativePosition + gameObject->parent->transform->worldPosition) * gameObject->transform->worldScale;
		gameObject->transform->relativePosition = gameObject->transform->relativePosition * gameObject->transform->worldScale;
	}


	for (GameObject* child : gameObject->children) {
		child->transform->worldScale = child->transform->scale * child->parent->transform->worldScale;
		gameObject->transform->worldPosition = (gameObject->transform->relativePosition + gameObject->parent->transform->worldPosition) * gameObject->transform->worldScale;
		gameObject->transform->relativePosition = gameObject->transform->relativePosition * gameObject->transform->worldScale;
		UpdateChildrenScale(child);
	}
	*/
}

void Transform::UpdateChildrenScale() {
	if (gameObject != nullptr)
		UpdateChildrenScale(gameObject);
}
