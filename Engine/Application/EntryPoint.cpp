#include "EntryPoint.h"
#include <GLFW/glfw3.h>

#include "Engine/Application/Application.h"
#include "Engine/Core/Time.h"
#include "Engine/Core/Skybox.h"
#include "Editor/SessionCache/Cache.h"

ApplicationClass* Plaza::Application = new Plaza::ApplicationClass();

using namespace Plaza;


#include "Editor/DefaultAssets/Models/DefaultModels.h"

#include "Engine/Vendor/Tracy/tracy/Tracy.hpp"

#include "Engine/Core/Physics.h"
#include "Engine/Core/Scripting/Mono.h"
#include "Editor/Project.h"
#include "Engine/Core/Scene.h"

#ifdef TRACY_ENABLE
int main() {
#ifdef GAME_REL != 0
	wchar_t buffer2[MAX_PATH];
	GetModuleFileNameW(NULL, buffer2, MAX_PATH);

	// Initialize a character buffer
	char exePath2[MAX_PATH];
	size_t convertedChars2 = 0;

	// Convert the wide-character string to a regular string safely
	if (wcstombs_s(&convertedChars2, exePath2, MAX_PATH, buffer2, MAX_PATH) == 0) {
		std::string exePathStr2(exePath2);
		for (auto const& entry : std::filesystem::directory_iterator{ std::filesystem::path{exePathStr2}.parent_path() })
		{
			if (entry.path().extension() == Standards::projectExtName) {
				std::cout << "Pathzao : " << entry.path().string() << "\n";
				Application->projectPath = entry.path().parent_path().string();
				std::cout << "ProjectPath1  : " << Application->projectPath << "\n";
			}
		}
	}

#endif

	// Start
	std::cout << "Creating Application \n";
	Application->CreateApplication();
	std::cout << "Initializating Physics \n";
	Physics::Init();
	Application->activeScene->mainSceneEntity = new Entity("Scene");
	Editor::DefaultModels::Init();

	//for (auto const& dir_entry : std::filesystem::directory_iterator{ sandbox })
	//	std::cout << dir_entry.path() << '\n';


	std::cout << "Loading Project \n";
#ifdef GAME_REL != 0
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);

	// Initialize a character buffer
	char exePath[MAX_PATH];
	size_t convertedChars = 0;

	// Convert the wide-character string to a regular string safely
	if (wcstombs_s(&convertedChars, exePath, MAX_PATH, buffer, MAX_PATH) == 0) {
		std::string exePathStr(exePath);
		for (auto const& entry : std::filesystem::directory_iterator{ std::filesystem::path{exePathStr}.parent_path() })
		{
			if (entry.path().extension() == Standards::projectExtName) {
				Editor::Project::Load(entry.path().string());

				std::cout << "Starting Scene\n";
				Scene::Play();
				Application->activeCamera = &Application->activeScene->cameraComponents.begin()->second;
			}
		}
	}
	else {
		// Handle conversion error
		std::cerr << "Error converting wide char to char" << std::endl;
	}


#else
	if (filesystem::exists(Application->enginePathAppData + "cache.yaml"))
		Editor::Cache::Load();
#endif
	Skybox::Init();

	
	std::cout << "Starting Loop \n";
	Application->Loop();
	Application->Terminate();
	return 0;
#
}
#endif