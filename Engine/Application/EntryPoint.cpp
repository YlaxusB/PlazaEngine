#include <GLFW/glfw3.h>

#include "Engine/Application/Application.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Skybox.h"

	int main() {
		Engine::Application::CreateApplication();
		Engine::Application::Loop();
		Engine::Application::Terminate();
		return 0;
	}