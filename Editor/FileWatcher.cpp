#include "Engine/Core/PreCompiledHeaders.h"
#include "Filewatcher.h"
#include "Engine/Vendor/Filewatcher/Filewatcher.h"
#include "Editor/ScriptManager/ScriptManager.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
namespace Plaza::Editor {
	std::vector<std::function<void()>> Filewatcher::mMainThreadQueue = std::vector<std::function<void()>>();
	void Filewatcher::Start(std::string pathToWatch) {
		filewatch::FileWatch<std::string>* watch = new filewatch::FileWatch<std::string>(pathToWatch, [pathToWatch](const std::string path, const filewatch::Event changeType) {
			//std::cout << path << " - ";
			std::filesystem::path fsPath = std::filesystem::path{ path };
			std::string finalPath = pathToWatch + "\\" + path;
			switch (changeType)
			{
			case filewatch::Event::added: // The file was added to the directory
				if (fsPath.extension() == ".cs") {
					Application->activeProject->scripts.emplace(pathToWatch + "\\" + path, Script());
				}
				break;
			case filewatch::Event::removed: // The file was removed from the directory
				if (fsPath.extension() == ".cs") {
					Application->activeProject->scripts.erase(pathToWatch + "\\" + path);
				}
				break;
			case filewatch::Event::modified: // The file was modified. This can be a change in the time stamp or attributes
				if (fsPath.extension() == ".cs") {
					if(Filewatcher::mMainThreadQueue.size() <= 0)
					Filewatcher::AddToMainThread([finalPath]() {ScriptManager::ReloadSpecificAssembly(finalPath); });
				}
				break;
			case filewatch::Event::renamed_old: // The file was renamed and this is the old name
				if (fsPath.extension() == ".cs") {
					//Application->activeProject->scripts.erase(pathToWatch + "\\" + path);
				}
				break;
			case filewatch::Event::renamed_new: // The file was renamed and this is the new name
				if (fsPath.extension() == ".cs") {
					//Application->activeProject->scripts.emplace(pathToWatch + "\\" + path, Script());
				}
				break;
			};
			//Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
			Gui::canUpdateContent = true;
			});
	}

	void Filewatcher::UpdateOnMainThread() {
		for (auto& function : mMainThreadQueue)
			function();
		mMainThreadQueue.clear();
	}

	void Filewatcher::AddToMainThread(const std::function<void()>& function) {
		mMainThreadQueue.push_back(function);
	}
}