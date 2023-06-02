#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Engine/Components/Core/Component.h"
#include "Engine/Components/Core/GameObject.h"
class GameObject;
class Transform : public Component {
public:
	//GameObject* gameObject;
	glm::vec3 position = { 0,0,0 };
	glm::vec3 worldPosition = { 0, 0, 0 };
	glm::vec3 relativePosition = { 0, 0, 0 };
	glm::vec3 rotation = { 0,0,0 };
	glm::vec3 scale = { 1,1,1, };
	glm::vec3 relativeScale = {1, 1, 1};

	glm::mat4 GetTransform(glm::vec3 worldp) const
	{
		//		glm::vec3 asd = glm::eulerToRadians(rotation);
		glm::mat4 rotation = glm::mat4(glm::quat(this->rotation));
		return glm::translate(glm::mat4(1.0f), worldp)
			* rotation
			* glm::scale(glm::mat4(1.0f), scale);
	}
};
#endif