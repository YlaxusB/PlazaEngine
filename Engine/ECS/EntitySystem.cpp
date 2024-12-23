#include "Engine/Core/PreCompiledHeaders.h"
#include "ECSManager.h"

namespace Plaza {
	uint64_t ECS::EntitySystem::Instantiate(Scene* scene, uint64_t uuidToInstantiate) {
		PLAZA_PROFILE_SECTION("Instantiate");
		if (scene->entities.find(uuidToInstantiate) == scene->entities.end())
			return 0;
		Entity* entityToInstantiate = &scene->entities.find(uuidToInstantiate)->second;
		Entity* instantiatedEntity = new Entity(entityToInstantiate->name, &scene->entities.find(entityToInstantiate->parentUuid)->second);
		instantiatedEntity = &scene->entities.find(instantiatedEntity->uuid)->second;
		uint64_t newUuid = instantiatedEntity->uuid;

		for (auto& pool : scene->mComponentPools) {
			if (!entityToInstantiate->mComponentMask.test(pool->mComponentId))
				continue;
			Component* component = (Component*)pool->Get(newUuid);
			component = nullptr;
			std::cout << component->mUuid << "\n";
			// FIX: Implement proper component instantiation (yes, the code above will crash the application, it is a big reminder to fix it)
		}

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
