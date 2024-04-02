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
		mDescription.radius = 1.0f;
		mDescription.density = 1.0f;
		mDescription.height = 2.0f;
		mDescription.maxJumpHeight = 1.0f;
		mDescription.slopeLimit = 1.0f;
		mDescription.material = Physics::defaultMaterial;
		mCharacterController = Physics::m_controllerManager->createController(mDescription);

	}

	void CharacterController::Update() {
		this->GetGameObject()->GetComponent<Transform>()->SetRelativePosition(glm::vec3((float)this->mCharacterController->getPosition().x, (float)this->mCharacterController->getPosition().y, (float)this->mCharacterController->getPosition().z));
	}

	void CharacterController::Move(glm::vec3 position) {
		mCharacterController->move(physx::PxVec3(position.x, position.y, position.z), 5.0f, 1.0f, nullptr);
	}
}