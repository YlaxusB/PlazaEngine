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

		if (Application->runningScene)
			this->Init();
	}

	void CharacterController::Init() {
		mDescription.radius = 0.5f;
		mDescription.density = 1.0f;
		mDescription.height = 1.0f;
		mDescription.maxJumpHeight = 1.0f;
		mDescription.slopeLimit = 1.0f;
		mDescription.contactOffset = 0.1f;
		mDescription.material = Physics::defaultMaterial;
		
		mCharacterController = Physics::m_controllerManager->createController(mDescription);
		mCharacterController->setContactOffset(0.08f);

		lastMoveTime = std::chrono::system_clock::now();
	}

	void CharacterController::Update() {
		this->GetGameObject()->GetComponent<Transform>()->SetRelativePosition(glm::vec3((float)this->mCharacterController->getPosition().x, (float)this->mCharacterController->getPosition().y, (float)this->mCharacterController->getPosition().z));
		this->hasMovedInThisFrame = false;
	}

	void CharacterController::Move(glm::vec3 position, float minimumDistance, bool followOrientation, float elapsedTime) {
		glm::vec3 finalPosition = position;
		if (followOrientation) {
			this->GetGameObject()->GetComponent<Transform>()->UpdateWorldMatrix();
			glm::mat4 matrix = this->GetGameObject()->GetComponent<Transform>()->GetTransform();
			glm::vec3 currentPosition = glm::vec3(matrix[3]);
			glm::vec3 forwardVector = glm::normalize(glm::vec3(matrix[2]));
			glm::vec3 leftVector = glm::normalize(glm::cross(glm::vec3(matrix[1]), forwardVector));
			glm::vec3 upVector = glm::normalize(glm::vec3(matrix[1]));
			finalPosition = forwardVector * position.x + leftVector * position.z + upVector * position.y;
			if (finalPosition.x != 0.0f)
				finalPosition.x = finalPosition.x;
		}
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		bool elapsedTimeIsInfinity = elapsedTime == std::numeric_limits<float>::infinity();
		mCharacterController->move(physx::PxVec3(finalPosition.x, finalPosition.y, finalPosition.z), minimumDistance, elapsedTimeIsInfinity ? (this->lastMoveTime - now).count() : 0.0f, nullptr);
		this->lastMoveTime = now;
		this->hasMovedInThisFrame = true;
		this->Update();
	}
}