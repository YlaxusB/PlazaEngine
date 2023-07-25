#include "Engine/Core/PreCompiledHeaders.h"
#include "ProjectManager.h"

#include "Engine/Application/FileDialog/FileDialog.h"
#include "Engine/Application/Serializer/SceneSerializer.h"
#include "Engine/Application/Serializer/ProjectSerializer.h"
namespace Engine {
	namespace Editor {
		namespace fs = std::filesystem;
		void ProjectManagerGui::OpenProjectClick() {
			std::string filePath = FileDialog::OpenFolderDialog();
			// If user didnt canceled, loop through all items found on the directory path and check for the first that haves the project extension. 
			if (filePath != "") {
				for (const auto& entry : fs::directory_iterator(filePath)) {
					std::string fileName = entry.path().filename().string();
					std::string extension = entry.path().extension().string();
					// Check if its a folder
					if (extension == Standards::projectExtName) {
						std::cout << filePath + "\\" + fileName << std::endl;
						ProjectSerializer::DeSerialize(filePath + "\\" + fileName);

						Application->activeProject = new Project();
						Application->activeProject->name = fileName;
						Application->activeProject->directory = filePath;

						Application->runEngine = true;
						Application->runProjectManagerGui = false;

						this->currentContent = new NewProjectContent();


						break;
					}
				}

			}

		}



	}
}
