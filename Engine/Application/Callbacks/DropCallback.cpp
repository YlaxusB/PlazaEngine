#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Application/ModelLoader.h"
#include "Engine/Application/Application.h"
void Engine::ApplicationClass::Callbacks::dropCallback(GLFWwindow* window, int count, const char** paths) {
	int i;
	for (i = 0; i < count; i++) {
		std::string fileParent = filesystem::path{ paths[i] }.parent_path().string();


		std::string fileName = filesystem::path{ paths[i] }.filename().string();
		ModelLoader::loadModel(paths[i], fileName);
		std::cout << "finished loading" << std::endl;
	}
}