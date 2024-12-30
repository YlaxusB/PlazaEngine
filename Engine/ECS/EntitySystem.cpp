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
		Entity* instantiatedEntity = scene->NewEntity(entityToInstantiate->name, scene->GetEntity(entityToInstantiate->parentUuid));
		uint64_t newUuid = instantiatedEntity->uuid;

		for (auto& pool : scene->mComponentPools) {
			if (!pool || !entityToInstantiate->mComponentMask.test(pool->mComponentMask))
				continue;
			Component* component = (Component*)pool->mInstantiateFactory(pool, pool->Get(uuidToInstantiate), newUuid);
			instantiatedEntity->mComponentMask.set(pool->mComponentMask);
		}
		for (auto& pool : scene->mComponentPools) {
			if (!pool || !entityToInstantiate->mComponentMask.test(pool->mComponentMask) || pool->Get(newUuid) == nullptr)
				continue;
			Component* component = static_cast<Component*>(pool->Get(newUuid));
			if (component->mUuid != newUuid)
				continue;
			component->OnInstantiate(scene, uuidToInstantiate);
		}

		/* Instantiate children */
		for (unsigned int i = 0; i < entityToInstantiate->childrenUuid.size(); i++) {
			uint64_t childUuid = entityToInstantiate->childrenUuid[i];
			uint64_t uuid = ECS::EntitySystem::Instantiate(scene, childUuid);
			if (uuid) {
				scene->SetParent(scene->GetEntity(uuid), scene->GetEntity(newUuid));
				ECS::TransformSystem::UpdateSelfAndChildrenTransform(*scene->GetComponent<TransformComponent>(uuid), scene->GetComponent<TransformComponent>(newUuid), scene);
			}
		}

		return instantiatedEntity->uuid;
	}

	void ECS::EntitySystem::Delete(Scene* scene, uint64_t uuid) {
		// FIX: Implement proper deletion of entites
		for (auto& componentPool : scene->mComponentPools) {

		}
	}

}
