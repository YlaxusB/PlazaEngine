#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "FilesManager.h"

namespace Plaza {
	std::filesystem::path FilesManager::CopyPasteFile(const std::filesystem::path& from, const std::filesystem::path& to, bool override) {
		if (!FilesManager::PathExists(from))
			return "";

		std::filesystem::copy_file(from, override ? to : FilesManager::GetValidPath(to), override ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::none);
		return to;
	}

	std::filesystem::path FilesManager::CreateFileCopy(const std::filesystem::path& from, bool override) {
		std::filesystem::path newDestinationPath = from;
		newDestinationPath = newDestinationPath.parent_path() / (from.stem().string() + "Copy" + from.extension().string());
		return FilesManager::CopyPasteFile(from, newDestinationPath, override);
	}

	std::filesystem::path FilesManager::GetValidPath(const std::filesystem::path& path) {
		int filesWithSameNameCount = FilesManager::GetNumberOfSameNameFiles(path.parent_path(), path.stem().string());
		if (filesWithSameNameCount == 0)
			return path;
		else {
			std::string newFileName = path.stem().string() + "(" + std::to_string(filesWithSameNameCount) + ")" + path.extension().string();
			return path.parent_path() / newFileName;
		}
	}

	int FilesManager::GetNumberOfSameNameFiles(const std::filesystem::path& path, const std::string& fileName) {
		std::filesystem::path directory(path);
		int count = 0;

		for (const auto& entry : std::filesystem::directory_iterator(directory)) {
			std::string entryName = entry.path().stem().string();
			if (entryName == fileName || (entryName.starts_with(fileName) && entryName.ends_with(")"))) {
				count++;
			}
		}

		return count;
	}

	bool FilesManager::PathExists(const std::filesystem::path& path) {
		return std::filesystem::exists(path);
	}

	bool FilesManager::PathMustExist(const std::filesystem::path& path) {
		if (!FilesManager::PathExists(path)) {
			PL_CORE_CRITICAL("Path does not exists");
			return false;
		}
		return true;
	}
}