#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
using namespace Engine;

void ApplicationClass::Callbacks::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	std::filesystem::path currentPath(__FILE__);
	std::string projectDirectory = currentPath.parent_path().parent_path().parent_path().string();

	if (key == GLFW_KEY_G && action == GLFW_PRESS)
		Application->Shadows->showDepth = !Application->Shadows->showDepth;

	if(key == GLFW_KEY_R && action == GLFW_PRESS)
		Application->shader = new Shader((projectDirectory + "\\Shaders\\1.model_loadingVertex.glsl").c_str(), (projectDirectory + "\\Shaders\\1.model_loadingFragment.glsl").c_str());

}