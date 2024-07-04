#include "Engine/Core/PreCompiledHeaders.h"
#include "Prefab.h"

namespace Plaza {
	Prefab::Prefab(Entity& rootEntity) {
		//this->mRootEntity = rootEntity;
		this->mPrefabName = rootEntity.name;


	}
}