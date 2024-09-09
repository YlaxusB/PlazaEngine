#define PX_PHYSX_STATIC_LIB
#include <Engine/Core/PreCompiledHeaders.h>
#include "EntryPoint.h"
#include <ThirdParty/GLFW/include/GLFW/glfw3.h>

#include "Engine/Application/Application.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Skybox.h"
#include "Editor/SessionCache/Cache.h"
using namespace Plaza;

#define TRACY_NO_INVARIANT_CHECK 1
#include "Editor/DefaultAssets/Models/DefaultModels.h"

#include "Engine/Core/Physics.h"
#include "Engine/Core/Scripting/Mono.h"
#include "Editor/Project.h"
#include "Engine/Core/Scene.h"
#include "Engine/Core/Audio/Audio.h"
#include "Editor/Settings/SettingsSerializer.h"

#include "Engine/Application/Serializer/ModelSerializer.h"

#include <windows.h>
#include <codecvt>

int main() {
	Log::Init();
	PL_CORE_INFO("Start");
	Application::Init();
	Application::Get()->CreateApplication();
	Application::Get()->Loop();
	Application::Get()->Terminate();
	return 0;
#
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
) {
	main();
}