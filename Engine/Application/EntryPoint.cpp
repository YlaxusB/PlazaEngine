#include "EntryPoint.h"
#include <GLFW/glfw3.h>

#include "Engine/Application/Application.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Skybox.h"

ApplicationClass* Engine::Application = new Engine::ApplicationClass();
using namespace Engine;
int main() {
	//Application = new Engine::ApplicationClass();
	std::cout << "1" << std::endl;
	Application->CreateApplication();
	std::cout << "2" << std::endl;
	Application->Loop();
	std::cout << "3" << std::endl;
	Application->Terminate();
	std::cout << "4" << std::endl;
	return 0;
}