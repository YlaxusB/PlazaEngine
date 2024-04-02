#include "Engine/Core/PreCompiledHeaders.h"
#include "CharacterController.h"
#include "Engine/Core/Physics.h"
#include "Collider.h"

#include <ThirdParty/PhysX/physx/include/characterkinematic/PxController.h>
#include <ThirdParty/PhysX/physx/include/characterkinematic/PxControllerManager.h>
#include <ThirdParty/PhysX/physx/include/characterkinematic/PxControllerBehavior.h>
#include <ThirdParty/PhysX/physx/include/characterkinematic/PxBoxController.h>
using namespace physx;
namespace Plaza {
	CharacterController::CharacterController(uint64_t uuid) {
		this->uuid = uuid != 0 ? uuid : Plaza::UUID::NewUUID();
	}

	void CharacterController::Init() {
		if (!Physics::m_controllerManager)
			Physics::m_controllerManager = PxCreateControllerManager(*Physics::m_scene);
		mCharacterController = Physics::m_controllerManager->createController(mDescription);
	}
}