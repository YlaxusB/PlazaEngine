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



#include "Engine/Core/ModelLoader/ModelLoader.h"
#include "Engine/Application/Serializer/ModelSerializer.h"

#include <windows.h>
#include <codecvt>
int main() {
	// Buffer to hold the path to the .exe
	wchar_t exePath3[MAX_PATH];

	// Get the path to the .exe
	GetModuleFileNameW(NULL, exePath3, MAX_PATH);

	// Extract the directory path (excluding the filename)
	std::wstring exeDirectory3(exePath3);
	size_t lastSlashPos = exeDirectory3.find_last_of(L"\\");
	exeDirectory3 = exeDirectory3.substr(0, lastSlashPos);

	// Append "\\Dlls" to the directory path
	std::wstring dllFolderPath = exeDirectory3 + L"\\Dlls";

	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	Application->exeDirectory = converter.to_bytes(exeDirectory3);
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
				Application->projectPath = entry.path().parent_path().string();
			}
		}
	}

#endif

	// Start
	std::cout << "Creating Application \n";
	Application->CreateApplication();

	/* Load Editor settings */
#ifndef GAME_REL
	Editor::EditorSettingsSerializer::DeSerialize();
#endif // GAME_REL

	/* Initialize Audio */
	Audio::Init();

	std::cout << "Initializating Physics \n";
	Physics::Init();
	Application->activeScene->mainSceneEntity = new Entity("Scene");
	////   Editor::DefaultModels::Init();

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
				std::cout << "Scene Played \n";
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
	if (Application->mRenderer->api == RendererAPI::OpenGL)
		OpenGLSkybox::Init();

	/*  -----   TEMPORARILY LOAD MODEL   -----  */
	// ModelLoader::LoadImportedModelToScene(ModelSerializer::ReadUUID("C:\\Users\\Giovane\\Desktop\\Workspace\\PlazaGames\\SpaceGame\\Assets\\Models\\sponza\\sponza.plzmod"), "C:\\Users\\Giovane\\Desktop\\Workspace\\PlazaGames\\SpaceGame\\Assets\\Models\\sponza\\sponza.plzmod");
	// ModelLoader::LoadImportedModelToScene(ModelSerializer::ReadUUID("C:\\Users\\Giovane\\Desktop\\Workspace\\PlazaGames\\SpaceGame\\Assets\\Models\\sponza\\sponza.plzmod"), "C:\\Users\\Giovane\\Desktop\\Workspace\\PlazaGames\\SpaceGame\\Assets\\Models\\sponza\\sponza.plzmod");

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