#include "EntryPoint.h"
#include <GLFW/glfw3.h>

#include "Engine/Application/Application.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Skybox.h"

ApplicationClass* Engine::Application = new Engine::ApplicationClass();
using namespace Engine;


#include "Editor/DefaultAssets/Models/DefaultModels.h"

#include "Engine/Vendor/Tracy/tracy/Tracy.hpp"
#ifdef TRACY_ENABLE
int main() {
	//Application = new Engine::ApplicationClass();
	Application->CreateApplication();

	Editor::DefaultModels::Init();
	Application->Loop();
	Application->Terminate();
	return 0;
}
#endif