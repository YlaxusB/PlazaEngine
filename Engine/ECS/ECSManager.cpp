#include "Engine/Core/PreCompiledHeaders.h"
#include "ECSManager.h"
#include "Engine/Core/Engine.h"
#include <ThirdParty/cereal/cereal/archives/json.hpp>
#include <ThirdParty/cereal/cereal/types/string.hpp>
#include <ThirdParty/cereal/cereal/types/vector.hpp>
#include <ThirdParty/cereal/cereal/cereal.hpp>
#include <ThirdParty/cereal/cereal/archives/binary.hpp>
#include <ThirdParty/cereal/cereal/types/map.hpp>
#include <ThirdParty/cereal/cereal/types/polymorphic.hpp>
#include <ThirdParty/cereal/cereal/types/utility.hpp>
#include "Engine/Components/Core/Transform.h"
#include "Engine/Core/Scene.h"

#define PL_REGISTER_COMPONENT(T) \
    CEREAL_REGISTER_TYPE(T) ;\
	CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, T) ;
namespace Plaza {
	template<typename T>
	void RegisterComponent() {
		const int componentId = Scene::GetComponentId<T>();
		if (componentId >= ECS::sInstantiateComponentFactory.size())
			ECS::sInstantiateComponentFactory.resize(componentId + 1);
		ECS::sInstantiateComponentFactory[componentId] = [](ComponentPool* srcPool, ComponentPool* dstPool, uint64_t srcUuid, uint64_t dstUuid) -> void* {
			T* component = dstPool->New<T>(dstUuid);
			*component = *static_cast<T*>(srcPool->Get(srcUuid));
			static_cast<Component*>(component)->mUuid = dstUuid;
			return component;
			};
	}

	void ECS::RegisterComponents() {
		Scene::sComponentCounter = 0;
		RegisterComponent<TransformComponent>();
		RegisterComponent<MeshRenderer>();
		RegisterComponent<Collider>();
		RegisterComponent<RigidBody>();
		RegisterComponent<Camera>();
		RegisterComponent<Light>();
		RegisterComponent<AudioSource>();
		RegisterComponent<AudioListener>();
		RegisterComponent<CppScriptComponent>();
		RegisterComponent<AnimationComponent>();
		RegisterComponent<CsScriptComponent>();
	}

	void ECS::InstantiateComponent(ComponentPool* srcPool, ComponentPool* dstPool, uint64_t srcUuid, uint64_t dstUuid) {
		Component* component = static_cast<Component*>(sInstantiateComponentFactory[srcPool->mComponentMask](srcPool, dstPool, srcUuid, dstUuid));
		component->mUuid = dstUuid;
	}
}
PL_REGISTER_COMPONENT(TransformComponent);
PL_REGISTER_COMPONENT(MeshRenderer);
PL_REGISTER_COMPONENT(Collider);
PL_REGISTER_COMPONENT(RigidBody);
PL_REGISTER_COMPONENT(Camera);
PL_REGISTER_COMPONENT(Light);
PL_REGISTER_COMPONENT(AudioSource);
PL_REGISTER_COMPONENT(AudioListener);
PL_REGISTER_COMPONENT(CppScriptComponent);
PL_REGISTER_COMPONENT(AnimationComponent);
PL_REGISTER_COMPONENT(CsScriptComponent);