#include "Engine/Core/PreCompiledHeaders.h"
#include "ECSManager.h"

namespace Plaza {
	void ECS::EntitySystem::SetParent(Scene* scene, Entity* child, Entity* newParent) {
		uint64_t oldParentUuid = child->parentUuid;
		if (oldParentUuid == newParent->uuid)
			return;

		Entity* oldParent = scene->GetEntity(oldParentUuid);
		if (oldParent) {
			bool haveChild = std::find(oldParent->childrenUuid.begin(), oldParent->childrenUuid.end(), child->uuid) != oldParent->childrenUuid.end();
			if (haveChild)
				oldParent->childrenUuid.erase(std::find(oldParent->childrenUuid.begin(), oldParent->childrenUuid.end(), child->uuid));
		}

		child->parentUuid = newParent->uuid;
		newParent->childrenUuid.push_back(child->uuid);
	}

	uint64_t ECS::EntitySystem::Instantiate(Scene* scene, uint64_t uuidToInstantiate) {
		PLAZA_PROFILE_SECTION("Instantiate");
		if (!scene->GetEntity(uuidToInstantiate))
			return 0;
		Entity* entityToInstantiate = scene->GetEntity(uuidToInstantiate);
		Entity* instantiatedEntity = scene->NewEntity(entityToInstantiate->name, scene->GetEntity(entityToInstantiate->parentUuid));//new Entity(entityToInstantiate->name, scene->GetEntity(entityToInstantiate->parentUuid));
		instantiatedEntity = scene->GetEntity(instantiatedEntity->uuid);
		uint64_t newUuid = instantiatedEntity->uuid;

		// FIX: Implement proper component instantiation
		//for (auto& pool : scene->mComponentPools) {
		//	if (!entityToInstantiate->mComponentMask.test(pool->mComponentId))
		//		continue;
		//	Component* component = (Component*)pool->Get(newUuid);
		//	component = nullptr;
		//	std::cout << component->mUuid << "\n";
		//	// FIX: Implement proper component instantiation (yes, the code above will crash the application, it is a big reminder to fix it)
		//}

		/* Instantiate children */
		for (unsigned int i = 0; i < entityToInstantiate->childrenUuid.size(); i++) {
			uint64_t childUuid = entityToInstantiate->childrenUuid[i];
			uint64_t uuid = ECS::EntitySystem::Instantiate(scene, childUuid);
			// FIX: Implement parent system with new DOD
			//if (uuid)
			//	Scene::GetActiveScene()->entities.at(uuid).ChangeParent(&Scene::GetActiveScene()->entities.at(uuid).GetParent(), instantiatedEntity);
		}

		return instantiatedEntity->uuid;
	}

	void ECS::EntitySystem::Delete(Scene* scene, uint64_t uuid) {
		// FIX: Implement proper deletion of entites
		for (auto& componentPool : scene->mComponentPools) {

		}
	}

}
