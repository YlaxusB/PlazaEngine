#include "Engine/Core/PreCompiledHeaders.h"
#include "RigidBody.h"
#include "Engine/Core/Physics.h"
#include "Collider.h"
#include "Editor/GUI/gizmo.h"
using namespace physx;
namespace Plaza {
	void RigidBody::OnInstantiate(Component* componentToInstantiate) {
		this->Init();
	}

	RigidBody::RigidBody(uint64_t uuid, bool initWithPhysics, bool dynamic) {
		if (initWithPhysics) {
			this->mUuid = uuid;
			this->dynamic = dynamic;
			Init();
		}
	}

	void RigidBody::Init() {
		// FIX: Rework Physics
	}
}
