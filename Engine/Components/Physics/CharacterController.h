#pragma once
#include "Engine/Components/Component.h"

namespace Plaza {
	class CharacterController : public Component {
	public:
		CharacterController(uint64_t uuid = 0);
		void Init();
		void Update();
		void Move(glm::vec3 position, float minimumDistance = 5.0f, bool followOrientation = true, float elapsedTime = 1.0f);
		physx::PxController* mCharacterController;
		physx::PxCapsuleControllerDesc mDescription = physx::PxCapsuleControllerDesc();

		bool hasMovedInThisFrame = false;

		std::chrono::system_clock::time_point lastMoveTime;
	};
}