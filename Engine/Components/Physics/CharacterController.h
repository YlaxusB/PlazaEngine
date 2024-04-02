#pragma once
#include "Engine/Components/Component.h"

namespace Plaza {
	class CharacterController : public Component {
	public:
		CharacterController(uint64_t uuid = 0);
		void Init();
		physx::PxController* mCharacterController;
		physx::PxCapsuleControllerDesc mDescription = physx::PxCapsuleControllerDesc();
	};
}