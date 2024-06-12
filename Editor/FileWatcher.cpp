#include "Engine/Core/PreCompiledHeaders.h"
#include "Filewatcher.h"
#include "Editor/ScriptManager/ScriptManager.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "Editor/GUI/FileExplorer/RenamedFile.h"
namespace Plaza::Editor {
	std::unordered_map<uint64_t, std::function<void()>> Filewatcher::mMainThreadQueue = std::unordered_map<uint64_t, std::function<void()>>();
	std::map< filewatch::Event, std::string> Filewatcher::mQueuedEvents = std::map<filewatch::Event, std::string>();
	std::queue<std::function<void()>> Filewatcher::taskQueue = std::queue<std::function<void()>>();
	std::mutex Filewatcher::queueMutex = std::mutex();
	void Filewatcher::Start(std::string pathToWatch) {
		filewatch::FileWatch<std::string>* watch = new filewatch::FileWatch<std::string>(pathToWatch, [pathToWatch](const std::string path, const filewatch::Event changeType) {
			//std::cout << path << " - ";
			std::filesystem::path fsPath = std::filesystem::path{ path };
			std::string finalPath = pathToWatch + "\\" + path;
			mQueuedEvents.emplace(changeType, finalPath);
			switch (changeType) {
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
					// TODO: For now its just recompiling the scripting but not hot reloading
					if (Filewatcher::mMainThreadQueue.size() <= 0) {
						Filewatcher::AddToMainThread([finalPath]() {
							ScriptManager::RecompileDll("", "");
							std::map<uint64_t, std::map<std::string, std::map<std::string, Field*>>> allFields = FieldManager::GetAllScritpsFields();
							Mono::OnStartAll(false);
							FieldManager::ApplyAllScritpsFields(allFields);
							});
						//Filewatcher::AddToMainThread([finalPath]() {ScriptManager::ReloadSpecificAssembly(finalPath); });
					}
					//Filewatcher::AddToMainThread([finalPath]() {ScriptManager::RecompileDll("", ""); });
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
			Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
			});
	}

	void Filewatcher::UpdateOnMainThread() {

		std::function<void()> task;

		{
			std::lock_guard<std::mutex> lock(queueMutex);

			if (!taskQueue.empty()) {
				task = taskQueue.front();
				taskQueue.pop();
			}
		}

		if (task) {
			task(); // Execute the task on the main thread
		}
		/* Execute queued functions*/
		//for (auto [key, function] : Filewatcher::mMainThreadQueue)
		//{
		//	function();
		//	//Filewatcher::mMainThreadQueue.erase(Filewatcher::mMainThreadQueue.find(key));
		//	//Filewatcher::mMainThreadQueue.erase(key);
		//	//mMainThreadQueue.erase(mMainThreadQueue.find(function));
		//}
		//Filewatcher::mMainThreadQueue.clear();
		/* Check the queued events, to check for renamed files */
		if (mQueuedEvents.size() >= 3) {
			std::string newPath;
			std::string oldPath;
			bool renamed = false;
			for (auto [event, path] : mQueuedEvents) {
				if (event == filewatch::Event::renamed_old || event == filewatch::Event::renamed_new || event == filewatch::Event::modified)
					renamed = true;
				else
					renamed = false;

				if (event == filewatch::Event::renamed_old)
					oldPath = path;
				if (event == filewatch::Event::renamed_new)
					newPath = path;
			}

			if (renamed) {
				Editor::RenamedFileManager::Run(oldPath, newPath);
			}
		}
		mQueuedEvents.clear();
	}

	void Filewatcher::AddToMainThread(const std::function<void()>& function) {
		//mMainThreadQueue.emplace(Plaza::UUID::NewUUID(), function);
		std::lock_guard<std::mutex> lock(queueMutex);
		taskQueue.push(function);
	}
}