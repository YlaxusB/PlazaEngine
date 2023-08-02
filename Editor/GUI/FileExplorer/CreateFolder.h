#pragma once
#include <string>
#include <filesystem>
#include "Engine/Utils/fileUtils.h"
using namespace Engine;
namespace Engine::Editor {

	string CreateFolder(std::string path, std::string name) {
		string newPath;
		if (std::filesystem::exists(path + "\\" + name)) {
			int sameNameFilesCount = FileUtils::GetNumberOfSameNameFiles(path, name);
			if (std::filesystem::exists(path + "\\" + name + " (" + std::to_string(sameNameFilesCount) + ")")) {
				sameNameFilesCount += 1;
			}
			newPath = path + "\\" + name + " (" + std::to_string(sameNameFilesCount) + ")";
			std::filesystem::create_directory(newPath);
		}
		else {
			newPath = path + "/" + name;
			std::filesystem::create_directory(newPath);
		}
		return newPath;
	}
	string CreateFolder(std::string fullPath) {
		string newPath;
		if (std::filesystem::exists(fullPath)) {
			newPath = fullPath + "(" + std::to_string(FileUtils::GetNumberOfSameNameFiles(fullPath, std::filesystem::path(fullPath).filename().string())) + ")";
			std::filesystem::create_directory(newPath);
		}
		else {
			newPath = fullPath;
			std::filesystem::create_directory(newPath);
		}
		return newPath;
	}


}