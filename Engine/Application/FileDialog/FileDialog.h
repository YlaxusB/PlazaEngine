#pragma once
#include <string>
#include <Commdlg.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include "ThirdParty//GLFW/glfw3native.h"
namespace Plaza {
	class FileDialog {
	public:
		static std::string OpenFolderDialog();
		static std::string OpenFileDialog(const char* filter);
		static std::string SaveFileDialog(const char* filter);
	};
}