#pragma once
#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtc/matrix_transform.hpp"
#include "ThirdParty/glm/gtc/type_ptr.hpp"


//#include "Engine/Components/Core/Entity.h"
#include "Engine/Components/Component.h"
#include "Engine/Core/Engine.h"

namespace Plaza {
	class PLAZA_API Transform : public Plaza::Component {
	public:
		bool haveCamera = false;
		glm::vec3 position = { 0,0,0 };
		glm::vec3 worldPosition = { 0, 0, 0 };
		glm::vec3 relativePosition = { 0, 0, 0 };
		glm::quat rotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 rotationEuler = glm::vec3(0.0f);
		glm::quat worldRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale = { 1,1,1, };
		glm::vec3 worldScale = { 1, 1, 1 };
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		glm::mat4 localMatrix = glm::mat4(1.0f);
		std::string scene = "Editor Scene";
		Transform();
		Transform(const Transform&) = default;
		const glm::quat& GetLocalQuaternion();
		const glm::vec3& GetLocalEuler();
		const glm::quat& GetWorldQuaternion();
		const glm::vec3& GetWorldEuler();
		const glm::mat4& GetTransform(glm::vec3 position, glm::vec3 scale);
		const glm::mat4& GetTransform(glm::vec3 position);
		const glm::mat4& GetTransform();
		const glm::mat4& GetLocalMatrix();
		void UpdateSelfAndChildrenTransform();
		void UpdateObjectTransform(Entity* entity);
		void UpdateChildrenTransform(Entity* entity);
		void UpdateChildrenTransform();
		void UpdateChildrenScale(Entity* entity);
		void UpdateChildrenScale();
		void MoveTowards(glm::vec3 vector);
		const glm::vec3& MoveTowardsReturn(glm::vec3 vector);

		void UpdateWorldMatrix();
		void UpdateLocalMatrix();

		void SetRelativePosition(glm::vec3 vector);
		void SetRelativeRotation(glm::quat quat);
		void SetRelativeScale(glm::vec3 vector);

		void SetWorldPosition(glm::vec3 vector);
		void SetWorldRotation(glm::vec3 vector);
		void SetWorldScale(glm::vec3 vector);

		void Rotate(glm::vec3 vector);

		const glm::vec3& GetWorldPosition();
		const glm::vec3& GetWorldRotation();
		const glm::vec3& GetWorldScale();

		void UpdatePhysics();

		template <class Archive>
		void serialize(Archive& archive) {
			archive(cereal::base_class<Component>(this), PL_SER(relativePosition), PL_SER(rotation), PL_SER(scale));
		}
	private:
		/* Matrices calculation cache */
		glm::mat4 lastLocalMatrix = glm::mat4(-1.0f);
		glm::vec3 lastRelativePositionLocalMatrix = glm::vec3(-1.0f);
		glm::quat lastRotationLocalMatrix = glm::quat(-1.0f, -1.0f, -1.0f, -1.0f);
		glm::vec3 lastScaleLocalMatrix = glm::vec3(-1.0f);

		glm::mat4 lastParentModelMatrix = glm::mat4(-1.0f);
		//this->localMatrix = glm::translate(glm::mat4(1.0f), this->relativePosition)
		//	* glm::toMat4(glm::quat(rotation))
		//	* glm::scale(glm::mat4(1.0f), scale);
	};
}