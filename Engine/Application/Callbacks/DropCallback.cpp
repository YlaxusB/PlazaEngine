#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Application/ModelLoader.h"
#include "Engine/Application/Application.h"
void Engine::ApplicationClass::Callbacks::dropCallback(GLFWwindow* window, int count, const char** paths) {
	int i;
	for (i = 0; i < count; i++) {
		std::string fileParent = filesystem::path{ paths[i] }.parent_path().string();
		vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
		vector<Mesh> meshes = vector<Mesh>();
		string directory = "";

		std::string fileName = filesystem::path{ paths[i] }.filename().string();
		ModelLoader::loadModel(paths[i], directory, &meshes, textures_loaded, fileName);
		std::cout << "finished loading" << std::endl;
	}
}