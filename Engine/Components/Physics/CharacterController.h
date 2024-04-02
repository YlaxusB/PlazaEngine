#pragma once
#include "Engine/Components/Component.h"

namespace Plaza {
	class CharacterController : public Component {
	public:
		CharacterController(uint64_t uuid = 0);
		void Init();
		void Update();
		void Move(glm::vec3 position);
		physx::PxController* mCharacterController;
		physx::PxCapsuleControllerDesc mDescription = physx::PxCapsuleControllerDesc();
	};
}