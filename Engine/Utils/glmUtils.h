#pragma once
#ifndef GLM_UTILS_H
#define GLM_UTILS_H

#include "Engine/Vendor/glm/glm.hpp"
#include "Engine/Vendor/glm/gtc/matrix_transform.hpp"
#include "Engine/Vendor/assimp/Importer.hpp"
#include "Engine/Vendor/assimp/scene.h"
#include "Engine/Vendor/assimp/postprocess.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

namespace glm {
	inline glm::vec3 eulerToRadians(glm::vec3 euler) {
		return glm::vec3(glm::radians(euler.x), glm::radians(euler.y), glm::radians(euler.z));
	}
}
#endif 