#pragma once
#include "Engine/Core/Time.h"
#include "Engine/Vendor/GLFW/glfw3.h"
#include <string>
#include <vector>
#include "Engine/Core/UUID.h"
#include "Engine/Vendor/glad/glad.h"


namespace Engine {
	class GameObject;
	class Component {
	public:
		uint64_t uuid;
		virtual ~Component() = default;
		GameObject* GetGameObject();
	};
}