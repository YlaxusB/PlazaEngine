#include "Engine/Core/PreCompiledHeaders.h"
#include "Mono.h"
namespace Plaza {
#define PL_ADD_INTERNAL_CALL(name) mono_add_internal_call("Plaza.InternalCalls::" #name, (void*)name)

#pragma region Components

#pragma region Transform Component
	static void SetPosition(glm::vec3* vec3) {
		if (Editor::selectedGameObject)
			Editor::selectedGameObject->GetComponent<Transform>()->SetRelativePosition(*vec3);
	}
	static void GetPositionCall(glm::vec3* out) {
		if (Editor::selectedGameObject)
			*out = Editor::selectedGameObject->GetComponent<Transform>()->relativePosition;
	}

	static bool InputIsKeyDown(int keyCode) {
		return glfwGetKey(Application->Window->glfwWindow, keyCode) == GLFW_PRESS ;
	}

	static void MoveTowards(glm::vec3 vector3) {
		if (Editor::selectedGameObject)
			Editor::selectedGameObject->GetComponent<Transform>()->MoveTowards(vector3);
	}

#pragma endregion Transform Component

#pragma region Mesh Renderer Component

#pragma endregion Mesh Renderer Component

#pragma endregion Components

	void InternalCalls::Init() {
		mono_add_internal_call("Plaza.InternalCalls::GetPositionCall", GetPositionCall);
		mono_add_internal_call("Plaza.InternalCalls::SetPosition", SetPosition);
		mono_add_internal_call("Plaza.InternalCalls::InputIsKeyDown", InputIsKeyDown);
		mono_add_internal_call("Plaza.InternalCalls::MoveTowards", MoveTowards);
		//PL_ADD_INTERNAL_CALL("GetPositionCall");
		// PL_ADD_INTERNAL_CALL("SetPosition");
	}

}