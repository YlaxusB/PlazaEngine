#pragma once
#include "Engine/Vendor/glm/glm.hpp"
namespace Engine {
	class ApplicationSizes {
	public:
		float appHeaderSize = 50.0f; // The header size above scene view
		glm::vec2 appSize = { 2560.0f, 1080.0f };
		glm::vec2 sceneSize = { 1920.0f, 768.0f };
		glm::vec2 sceneStart = { 0.0f, 0.0f };
		glm::vec2 hierarchySize = { 320, 1000 };
		glm::vec2 inspectorSize = { 420, 1000 };
		glm::vec2 sceneImageStart = { 0, 0 };
	};
}