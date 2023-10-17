#include "Engine/Core/PreCompiledHeaders.h"
#include "EditorSettings.h"
namespace Plaza::Editor {
	void Settings::ReapplyAllSettings() {
		if (vsync)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}
}