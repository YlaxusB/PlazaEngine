#pragma once
#include <string>
#include <Commdlg.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include "Engine/Vendor//GLFW/glfw3native.h"
namespace Engine {
	class FileDialog {
	public:
		static std::string OpenFileDialog(const char* filter);
		static std::string SaveFileDialog(const char* filter);
	};
}