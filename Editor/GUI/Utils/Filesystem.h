#pragma once	
#include <string>
#include <filesystem>
#include "Editor/GUI/FileExplorer/FileExplorer.h"
namespace Plaza::Editor::Utils {
	class Filesystem {
	public:
		static void CreateFolder(std::string fullPath) {
			std::filesystem::create_directory(fullPath);
			Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
		}

		static void CreateFolder(std::string path, std::string name) {
			CreateFolder(path + "\\" + name);
		}

		static std::ofstream CreateNewFile(std::string fullPath) {
			std::ofstream file(fullPath);
			Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
			return file;
		}

		static void CreateNewFile(std::string path, std::string name) {
			CreateNewFile(path + "\\" + name);
		}

		static void CreateNewFile(std::string path, std::string name, std::string extension) {
			CreateNewFile(path + "\\" + name + "." + extension);
		}
	};
}