#include "Engine/Core/PreCompiledHeaders.h"
#include "BoxCollider.h"
namespace Engine {
	void BoxCollider::Init() {

		Transform& transform = Application->activeScene->transformComponents.at(this->uuid);
		physx::PxBoxGeometry boxGeometry = physx::PxBoxGeometry(transform.scale.x / 2, transform.scale.y / 2, transform.scale.z / 2);
		//RigidBody* rigidBody = &Application->activeScene->rigidBodyComponents.at(this->uuid);
		physx::PxMaterial* defaultMaterial = Physics::m_physics->createMaterial(0.5f, 0.5f, 0.5f);
		this->m_shape = Physics::m_physics->createShape(physx::PxSphereGeometry(1.0f), &defaultMaterial, false);

	}

	void BoxCollider::Update() 
	{

	}
}