#include "EntryPoint.h"
#include <GLFW/glfw3.h>

#include "Engine/Application/Application.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Skybox.h"

int main() {
	Engine::ApplicationClass* Application = new Engine::ApplicationClass();
	Application->CreateApplication();
	Application->Loop();
	Application->Terminate();
	return 0;
}