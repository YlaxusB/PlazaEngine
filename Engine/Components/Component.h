#pragma once
#include "Engine/Core/Time.h"
#include "ThirdParty/GLFW/include/GLFW/glfw3.h"
#include <string>
#include <vector>
#include "Engine/Core/UUID.h"
#include "ThirdParty/glad/glad.h"


namespace Plaza {
	class Entity;
	class Component {
	public:
		uint64_t uuid;
		uint64_t componentUuid;
		Entity* GetGameObject();
		virtual ~Component() = default;
		Component() {
			this->componentUuid = Plaza::UUID::NewUUID();
		}

		template <class Archive>
		void serialize(Archive& archive) {
			archive(uuid);
		}
	};
}