#include "Engine/Core/PreCompiledHeaders.h"
#include "CppHelper.h"

namespace Plaza {
	Entity* CppHelper::FindEntity(const std::string& name) {
		return Scene::GetActiveScene()->GetEntityByName(name);
	}

	glm::vec3 CppHelper::MousePosToRayDirection(float fov, const glm::vec2& position, const glm::vec2& screenSize) {
		float aspect = screenSize.x / screenSize.y;
		glm::vec2 relative = glm::vec2(position.x / screenSize.x - 0.5f, position.y / screenSize.y - 0.5f);
		float verticalAngle = 0.5f * Mathf::sDeg2Rad * fov;
		float worldHeight = 2.0f * glm::tan(verticalAngle);
		glm::vec3 worldUnits = glm::vec3(relative.x * worldHeight, relative.y * worldHeight, 0.0f);
		worldUnits.x *= aspect;
		worldUnits.z = 1;
		return worldUnits;
	}
}