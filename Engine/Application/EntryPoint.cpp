#include "EntryPoint.h"
#include <GLFW/glfw3.h>

#include "Engine/Application/Application.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Skybox.h"

ApplicationClass* Engine::Application = new Engine::ApplicationClass();
using namespace Engine;
int main() {
	//Application = new Engine::ApplicationClass();
	Application->CreateApplication();
	Application->Loop();
	Application->Terminate();
	return 0;
}