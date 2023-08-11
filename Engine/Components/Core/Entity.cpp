#include "Engine/Core/PreCompiledHeaders.h"
#include "Entity.h"
#include "Engine/Core/UUID.h"
namespace Engine {
	Entity::Entity() {
		this->uuid = UUID::NewUUID();
	}
}