#include "Engine/Core/PreCompiledHeaders.h"
#include "Component.h"

namespace Engine {
	GameObject* Component::GetGameObject() {
		return &Application->activeScene->entities.at(this->uuid);
	}
}