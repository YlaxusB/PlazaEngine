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

		static void CreateNewFile(std::string fullPath) {
			std::ofstream file(fullPath);
			if (std::filesystem::path{ fullPath }.extension().string() == ".cpp") {
				if (file.is_open()) {
					std::string fileName = std::filesystem::path{ fullPath }.stem().string();
					file << "// Your C++ code goes here" << std::endl;
					file << "namespace Plaza {" << std::endl;
					file << "	class " << fileName << " : public CppScript{" << std::endl;
					file << "	public:" << std::endl;
					file << "		void OnStart()" << std::endl;
					file << "		{" << std::endl;
					file << "		" << std::endl;
					file << "		}" << std::endl;
					file << "		" << std::endl;
					file << "		void OnRestart()" << std::endl;
					file << "		{" << std::endl;
					file << "		" << std::endl;
					file << "		}" << std::endl;
					file << "		" << std::endl;
					file << "		void OnUpdate()" << std::endl;
					file << "		{" << std::endl;
					file << "		" << std::endl;
					file << "		}" << std::endl;
					file << "	}" << std::endl;
					file << "}" << std::endl;
					file.close();
				}
				else {
					std::cout << "Unable to create file." << std::endl;
				}
			}
			Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
		}

		static void CreateNewFile(std::string path, std::string name) {
			CreateNewFile(path + "\\" + name);
		}

		static void CreateNewFile(std::string path, std::string name, std::string extension) {
			CreateNewFile(path + "\\" + name + "." + extension);
		}
	};
}