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

#define PL_REGISTER_COMPONENT(T) \
    CEREAL_REGISTER_TYPE(T) ;\
	CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, T) ;
namespace Plaza {
	template<typename T>
	void RegisterComponent() {
		Scene::GetComponentId<T>();
	}

	void ECS::RegisterComponents() {
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