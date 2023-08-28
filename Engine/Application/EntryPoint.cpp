#include "EntryPoint.h"
#include <GLFW/glfw3.h>

#include "Engine/Application/Application.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Skybox.h"

ApplicationClass* Plaza::Application = new Plaza::ApplicationClass();

using namespace Plaza;


#include "Editor/DefaultAssets/Models/DefaultModels.h"

#include "Engine/Vendor/Tracy/tracy/Tracy.hpp"

#include "Engine/Core/Physics.h"
#include "Engine/Core/Mono.h"
#ifdef TRACY_ENABLE
int main() {
	// Start
	Application->CreateApplication();
	Physics::Init();
	Application->activeScene->mainSceneEntity = new Entity("Scene");
	Editor::DefaultModels::Init();
	Application->Loop();
	Application->Terminate();
	return 0;
}
#endif