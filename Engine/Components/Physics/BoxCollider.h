#pragma once
#include "Collider.h"
#include "Engine/Core/Physics.h"
namespace Engine {
	class BoxCollider : public Collider {
	public:

		void Init() override;

		void Update() override;
	};
}