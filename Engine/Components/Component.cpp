//#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Core/Scene.h"
#include "Component.h"
#include "Engine/Components/Core/Entity.h"

namespace Plaza {
	Entity* Component::GetGameObject() {
		return &Scene::GetActiveScene()->entities.find(this->mUuid)->second;
	}
}