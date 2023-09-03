#include "Engine/Core/PreCompiledHeaders.h"
#include "Mono.h"
namespace Plaza {

#define PL_ADD_INTERNAL_CALL(name) mono_add_internal_call("Plaza.InternalCalls::" #name, (void*)name)

	static bool HasComponent(uint64_t uuid, MonoReflectionType* componentType) {
		MonoType* monoType = mono_reflection_type_get_type(componentType);
		return Mono::mEntityHasComponentFunctions.at(monoType)(*Application->activeScene->GetEntity(uuid));
		//return Application->activeScene->entities.at(uuid).HasComponent<type>();
	}
#pragma region Components

#pragma region Transform Component
	static void SetPosition(uint64_t uuid, glm::vec3* vec3) {
		Application->activeScene->transformComponents.at(uuid).SetRelativePosition(*vec3);
	}
	static void GetPositionCall(uint64_t uuid, glm::vec3* out) {
		*out = Application->activeScene->transformComponents.at(uuid).relativePosition;
	}

	static bool InputIsKeyDown(int keyCode) {
		return glfwGetKey(Application->Window->glfwWindow, keyCode) == GLFW_PRESS;
	}

	static void MoveTowards(uint64_t uuid, glm::vec3 vector3) {
		auto transformIt = Application->activeScene->transformComponents.find(uuid);
		if (transformIt != Application->activeScene->transformComponents.end())
			transformIt->second.MoveTowards(vector3);

	}


#pragma endregion Transform Component

#pragma region Mesh Renderer Component

#pragma endregion Mesh Renderer Component

#pragma endregion Components

	void InternalCalls::Init() {
		//PL_ADD_INTERNAL_CALL(GetPositionCall);
		mono_add_internal_call("Plaza.InternalCalls::HasComponent", HasComponent);
		mono_add_internal_call("Plaza.InternalCalls::GetPositionCall", GetPositionCall);
		mono_add_internal_call("Plaza.InternalCalls::SetPosition", SetPosition);
		mono_add_internal_call("Plaza.InternalCalls::InputIsKeyDown", InputIsKeyDown);
		mono_add_internal_call("Plaza.InternalCalls::MoveTowards", MoveTowards);
		// PL_ADD_INTERNAL_CALL("SetPosition");
	}

}