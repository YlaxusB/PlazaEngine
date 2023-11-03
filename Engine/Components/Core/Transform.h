#pragma once
#include "Engine/Vendor/glm/glm.hpp"
#include "Engine/Vendor/glm/gtc/matrix_transform.hpp"
#include "Engine/Vendor/glm/gtc/type_ptr.hpp"


#include "Engine/Components/Core/Entity.h"
#include "Engine/Components/Component.h"

namespace Plaza {
	class Transform : public Plaza::Component {
	public:
		bool haveCamera = false;
		glm::vec3 position = { 0,0,0 };
		glm::vec3 worldPosition = { 0, 0, 0 };
		glm::vec3 relativePosition = { 0, 0, 0 };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 worldRotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1,1,1, };
		glm::vec3 worldScale = { 1, 1, 1 };
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 localMatrix = glm::mat4(1.0f);
		std::string scene = "Editor Scene";
		Transform();
		Transform(const Transform&) = default;
		glm::quat GetLocalQuaternion();
		glm::quat GetWorldQuaternion();
		glm::mat4 GetTransform(glm::vec3 position, glm::vec3 scale);
		glm::mat4 GetTransform(glm::vec3 position);
		glm::mat4 GetTransform();
		glm::mat4 GetLocalMatrix();
		void UpdateSelfAndChildrenTransform();
		void UpdateObjectTransform(Entity* entity);
		void UpdateChildrenTransform(Entity* entity);
		void UpdateChildrenTransform();
		void UpdateChildrenScale(Entity* entity);
		void UpdateChildrenScale();
		void MoveTowards(glm::vec3 vector);

		void UpdateWorldMatrix();
		void UpdateLocalMatrix();

		void SetRelativePosition(glm::vec3 vector);
		void SetRelativeRotation(glm::vec3 vector);
		void SetRelativeScale(glm::vec3 vector);
		
		void SetWorldPosition(glm::vec3 vector);
		void SetWorldRotation(glm::vec3 vector);
		void SetWorldScale(glm::vec3 vector);

		void Rotate(glm::vec3 vector);

		glm::vec3 GetWorldPosition();
		glm::vec3 GetWorldRotation();
		glm::vec3 GetWorldScale();

		void UpdatePhysics();

	private:
		/* Matrices calculation cache */
		glm::mat4 lastLocalMatrix = glm::mat4(-1.0f);
		glm::vec3 lastRelativePositionLocalMatrix = glm::vec3(-1.0f);
		glm::vec3 lastRotationLocalMatrix = glm::vec3(-1.0f);
		glm::vec3 lastScaleLocalMatrix = glm::vec3(-1.0f);

		glm::mat4 lastParentModelMatrix = glm::mat4(-1.0f);
		//this->localMatrix = glm::translate(glm::mat4(1.0f), this->relativePosition)
		//	* glm::toMat4(glm::quat(rotation))
		//	* glm::scale(glm::mat4(1.0f), scale);
	}; 
}