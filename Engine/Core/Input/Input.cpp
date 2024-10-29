#include "Engine/Core/PreCompiledHeaders.h"
#include "Input.h"
#include "Cursor.h"
namespace Plaza {
	void Input::Update() {
		Cursor::Update();
	}

	bool Input::GetKeyDown(int key) {
		return glfwGetKey(Application::Get()->mWindow->glfwWindow, key) == GLFW_PRESS;
	}

	bool Input::GetKeyDownOnce()
	{
		return false;
	}
	bool Input::GetKeyReleased()
	{
		return false;
	}
	bool Input::GetKeyReleasedOnce()
	{
		return false;
	}
}