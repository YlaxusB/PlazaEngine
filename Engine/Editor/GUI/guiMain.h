#pragma once
#ifndef guiHeader_H
#define guiHeader_H
#include "../../Application.h"

namespace gui {
	void setupDockspace(GLFWwindow* window, int gameFrameBuffer, AppSizes& appSizes, AppSizes& lastAppSizes);
}

namespace ImGui {
	inline bool Compare(ImVec2 firstVec, ImVec2 secondVec) {
		return firstVec.x == secondVec.x && firstVec.y == secondVec.y;
	}
	// Transforms glm::vec2 to ImVec2
	inline ImVec2 imVec2(glm::vec2 vec) {
		return ImVec2(vec.x, vec.y);
	}
	// Transforms ImVec2 to glm::vec2
	inline glm::vec2 glmVec2(ImVec2 imguiVec) {
		return glm::vec2(imguiVec.x, imguiVec.y);
	}
}

#endif // guiHeader_H