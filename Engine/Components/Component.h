#pragma once
#include "Engine/Core/Time.h"
#include "Engine/Vendor/GLFW/glfw3.h"
#include <string>
#include <vector>
#include "Engine/Core/UUID.h"
#include "Engine/Vendor/glad/glad.h"


namespace Plaza {
	class Entity;
	class Component {
	public:
		uint64_t uuid;
		Entity* GetGameObject();
		virtual ~Component() = default;
	};
}