#include "Engine/Core/PreCompiledHeaders.h"
#include "Input.h"
#include "Cursor.h"
namespace Plaza {
	bool Input::Cursor::show = true;
	double Input::Cursor::lastX = 0;
	double Input::Cursor::lastY = 0;
	double Input::Cursor::deltaX = 0;
	double Input::Cursor::deltaY = 0;
	void Input::Cursor::Update() {
		if (Application->focusedMenu == "Scene") {
			double currentX;
			double currentY;
			glfwGetCursorPos(Application->Window->glfwWindow, &currentX, &currentY);

			deltaX = currentX - lastX;
			deltaY = currentY - lastY;

			lastX = currentX;
			lastY = currentY;
		}
	}
}