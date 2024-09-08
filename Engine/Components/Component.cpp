#include "Engine/Core/PreCompiledHeaders.h"
#include "Component.h"

namespace Plaza {
	Entity* Component::GetGameObject() {
		return &Application::Get()->activeScene->entities.find(this->mUuid)->second;
	}
}