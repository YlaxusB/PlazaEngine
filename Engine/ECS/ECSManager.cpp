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
	void ECS::RegisterComponents() {
		Scene::sComponentCounter = 0;
		ECS::RegisterComponent<TransformComponent>();
		ECS::RegisterComponent<MeshRenderer>();
		ECS::RegisterComponent<Collider>();
		ECS::RegisterComponent<RigidBody>();
		ECS::RegisterComponent<Camera>();
		ECS::RegisterComponent<Light>();
		ECS::RegisterComponent<AudioSource>();
		ECS::RegisterComponent<AudioListener>();
		ECS::RegisterComponent<CppScriptComponent>();
		ECS::RegisterComponent<AnimationComponent>();
		ECS::RegisterComponent<CsScriptComponent>();
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