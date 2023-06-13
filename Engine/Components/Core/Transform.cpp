#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/trigonometric.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


#include "Engine/Components/Core/Transform.h"
//#include "Engine/Components/Core/GameObject.h"

glm::vec3 ConvertToRadians(const glm::vec3& anglesInDegrees);
Transform::Transform() {

}

glm::mat4 Transform::GetTransform(glm::vec3 pos)
{
	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, pos);
	//model = glm::rotate(model, glm::radians(worldRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, glm::radians(worldRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, glm::radians(worldRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

	//model = glm::scale(model, this->worldScale);
	//gizmoTransform = glm::scale(gizmoTransform, this->worldScale);
	//return model;

	/*
	glm::vec3 wrappedRotation = glm::mod(this->worldRotation, glm::vec3(360.0f));
	glm::mat4 rotation = glm::mat4(1.0f);

	rotation = glm::rotate(rotation, glm::radians(wrappedRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Yaw rotation
	rotation = glm::rotate(rotation, glm::radians(wrappedRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // Pitch rotation
	rotation = glm::rotate(rotation, glm::radians(wrappedRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Roll rotation
		//glm::mat4 rotation = glm::mat4(glm::quat(glm::eulerToRadians(this->worldRotation)));

		return glm::translate(glm::mat4(1.0f), pos)
			* rotation
			* glm::scale(glm::mat4(1.0f), this->worldScale);
	*/
	/*
	glm::mat4 gizmoTransform = glm::mat4(1.0f);
	glm::vec3 worldRotation = this->worldRotation;
	gizmoTransform = glm::translate(gizmoTransform, pos);
	gizmoTransform = glm::rotate(gizmoTransform, glm::radians(worldRotation.y), glm::vec3(0, 1, 0)); // Rotate around the Z-axis
	gizmoTransform = glm::rotate(gizmoTransform, glm::radians(worldRotation.x), glm::vec3(1, 0, 0)); // Rotate around the Y-axis
	gizmoTransform = glm::rotate(gizmoTransform, glm::radians(worldRotation.z), glm::vec3(0, 0, 1)); // Rotate around the X-axis


	* 	gizmoTransform = glm::rotate(gizmoTransform, glm::radians(this->worldRotation.z), glm::vec3(0, 0, 1)); // Rotate around the Z-axis
	gizmoTransform = glm::rotate(gizmoTransform, glm::radians(this->worldRotation.y), glm::vec3(0, 1, 0)); // Rotate around the Y-axis
	gizmoTransform = glm::rotate(gizmoTransform, glm::radians(this->worldRotation.x), glm::vec3(1, 0, 0)); // Rotate around the X-axis
	*
	*
				model = glm::rotate(model, glm::radians(worldRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(worldRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(worldRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	gizmoTransform = glm::scale(gizmoTransform, this->worldScale);
	return gizmoTransform;
	*/
	//glm::vec3 asd = this->rotation;
	//glm::vec3 rotation = asd;

	// Convert the rotation angles to radians
	///glm::vec3 rotationRadians(glm::radians(rotation.x),
	//	glm::radians(rotation.y),
	//	glm::radians(rotation.z));

	// Create a mat4 identity matrix
	//glm::mat4 rotationMatrix(1.0f);

	// Apply the rotation transformations
	//rotationMatrix = glm::translate(rotationMatrix, pos);
	//rotationMatrix = glm::rotate(rotationMatrix, rotationRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
	//rotationMatrix = glm::rotate(rotationMatrix, rotationRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
	//rotationMatrix = glm::rotate(rotationMatrix, rotationRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));
	//rotationMatrix = glm::scale(rotationMatrix, this->worldScale);
	//return rotationMatrix;
	
	glm::quat rotationQuat = glm::quat(glm::vec3(this->rotation.x, this->rotation.y, (this->rotation.z)));
	glm::mat4 rot = glm::toMat4(rotationQuat);
	this->rotation.z *= -1.0f;
	rotationQuat = glm::quat(rotationQuat.w, rotationQuat.x, rotationQuat.y, rotationQuat.z);
	return glm::translate(glm::mat4(1.0f), pos)
		* rot
		* glm::scale(glm::mat4(1.0f), scale * worldScale);
	
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
	glm::vec3 radiansRotation = glm::radians(child->parent->transform->worldRotation);
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, radiansRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 transformedPoint = glm::vec3(rotationMatrix * glm::vec4(child->transform->relativePosition, 1.0f));
	glm::vec3 finalWorldPoint = transformedPoint + child->parent->transform->worldPosition;
	//child->transform->worldPosition = finalWorldPoint;
	glm::vec3 localPoint = glm::vec3(rotationMatrix * glm::vec4(transformedPoint, 1.0f));
	return finalWorldPoint;
}

void Transform::UpdateChildrenTransform(GameObject* gameObject) {
	if (gameObject->parent != nullptr) {
		gameObject->transform->worldScale = gameObject->transform->scale * gameObject->parent->transform->worldScale;
		gameObject->transform->worldPosition = (gameObject->transform->relativePosition * gameObject->parent->transform->worldScale + gameObject->parent->transform->worldPosition);
		gameObject->transform->worldRotation = gameObject->transform->rotation + gameObject->parent->transform->worldRotation;
		gameObject->transform->worldPosition = test(gameObject);



		glm::vec3 radiansRotation = glm::radians(gameObject->parent->transform->worldRotation);
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
		child->transform->worldRotation = child->transform->rotation + child->parent->transform->worldRotation;
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
