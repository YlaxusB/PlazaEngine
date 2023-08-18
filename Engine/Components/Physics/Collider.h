#pragma once
#include "Engine/Vendor/physx/PxPhysicsAPI.h"
#include "Engine/Components/Component.h"
namespace Engine {
	class Collider : public Component {
	public:
		physx::PxShape* m_shape;
		virtual void Init() {};
		virtual void Update() {};
	};
}