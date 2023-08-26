#include "Engine/Core/PreCompiledHeaders.h"
#include "Component.h"

namespace Plaza {
	Entity* Component::GetGameObject() {
		return &Application->activeScene->entities.at(this->uuid);
	}
}