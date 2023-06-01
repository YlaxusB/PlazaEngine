#pragma once
#ifndef header_H
#define header_H
#include <glm/glm.hpp>
// Struct of all aplication gui sizes
struct AppSizes {
	float appHeaderSize = 25.0f; // The header size above scene view
	glm::vec2 appSize = { 2560.0f, 1080.0f };
	glm::vec2 sceneSize = { 1920.0f, 768.0f };
	glm::vec2 hierarchySize = { 320, 1000 };
	glm::vec2 inspectorSize = { 320, 1000 };
};
#endif // header_H