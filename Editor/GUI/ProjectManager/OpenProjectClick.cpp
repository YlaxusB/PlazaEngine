#include "Engine/Core/PreCompiledHeaders.h"
#include "ProjectManager.h"

#include "Engine/Application/FileDialog/FileDialog.h"
#include "Engine/Application/Serializer/SceneSerializer.h"
#include "Engine/Application/Serializer/ProjectSerializer.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "Engine/Application/Serializer/ScriptManagerSerializer.h"
#include "Engine/Core/Scripting/Mono.h"
#include "Editor/Filewatcher.h"
#include "Editor/ScriptManager/ScriptManager.h"
namespace Plaza {
	namespace Editor {
		namespace fs = std::filesystem;
		void ProjectManagerGui::OpenProjectClick() {
			std::string filePath = FileDialog::OpenFileDialog(Standards::projectExtName.c_str());
			// If user didnt canceled, loop through all items found on the directory path and check for the first that haves the project extension. 
			if (filePath != "") {
				fs::path projectFile(filePath);

				std::string fileName = projectFile.filename().string();
				std::string extension = projectFile.extension().string();
				// Check if its extension is the project extension
				if (extension == Standards::projectExtName) {
					std::cout << filePath << std::endl;
					ProjectSerializer::DeSerialize(filePath);

					Application->activeProject = new Project();
					Application->activeProject->name = fileName;
					Application->activeProject->directory = projectFile.parent_path().string();
					Application->activeProject->scriptsConfigFilePath = Application->activeProject->directory + "\\Scripts" + Standards::scriptConfigExtName;
					//Application->activeProject->scripts = ScriptManagerSerializer::DeSerialize(Application->activeProject->scriptsConfigFilePath);
					Filewatcher::Start(Application->activeProject->directory);

					/* Detect all the scripts in this folder */
					for (const auto& entry : fs::recursive_directory_iterator(Application->activeProject->directory)) {
						if (entry.is_regular_file() && entry.path().extension() == ".cs") {
							Application->activeProject->scripts.emplace(entry.path().string(), Script());
							// Reload all .dll files
							//ScriptManager::ReloadSpecificAssembly(entry.path().string());
						}
					}

					Application->runEngine = true;
					Application->runProjectManagerGui = false;

					this->currentContent = new NewProjectContent();
					Gui::FileExplorer::currentDirectory = Application->activeProject->directory;
					Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
					Mono::Init();
					return;
				}
				std::cout << "Project has not been found!" << "\n";
			}

		}



	}
}
