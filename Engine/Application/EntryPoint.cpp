#define PX_PHYSX_STATIC_LIB
#include <Engine/Core/PreCompiledHeaders.h>
#include "EntryPoint.h"
#include <ThirdParty/GLFW/include/GLFW/glfw3.h>

#include "Engine/Application/Application.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Skybox.h"
#include "Editor/SessionCache/Cache.h"

ApplicationClass* Plaza::Application = new Plaza::ApplicationClass();

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

	Application->exeDirectory = filesystem::current_path().string();

	AssetsManager::Init();

	Application->CreateApplication();

	Application->mThreadsManager->Init();

	/* Load Editor settings */
#ifdef EDITOR_MODE
	Editor::EditorSettingsSerializer::DeSerialize();
#endif

	std::cout << "Initializating Audio \n";
	Audio::Init();

	std::cout << "Initializating Physics \n";
	Physics::Init();
	Application->activeScene->mainSceneEntity = new Entity("Scene");

#ifdef GAME_MODE
	std::cout << "Loading Project \n";
	for (auto const& entry : std::filesystem::directory_iterator{ Application->exeDirectory }) {
		if (entry.path().extension() == Standards::projectExtName) {
			Editor::Project::Load(entry.path().string());

			std::cout << "Starting Scene\n";
			Scene::Play();
			std::cout << "Scene Played \n";
			if (Application->activeScene->cameraComponents.size() > 0)
				Application->activeCamera = &Application->activeScene->cameraComponents.begin()->second;
			else
				Application->activeCamera = Application->activeScene->mainSceneEntity->AddComponent<Camera>(new Camera());
		}
	}

#else
	if (filesystem::exists(Application->enginePathAppData + "cache.yaml"))
		Editor::Cache::Load();
#endif
	if (Application->mRenderer->api == RendererAPI::OpenGL)
		OpenGLSkybox::Init();

	std::cout << "Starting Loop \n";
	Application->Loop();
	std::cout << "Terminate \n";
	Application->Terminate();
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