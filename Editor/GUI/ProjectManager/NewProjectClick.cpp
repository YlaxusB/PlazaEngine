#include "Engine/Core/PreCompiledHeaders.h"
#include "ProjectManager.h"

#include "Engine/Application/FileDialog/FileDialog.h"
#include "Engine/Application/Serializer/SceneSerializer.h"

#include "Editor/Settings/ProjectGenerator.h"
namespace Plaza {
	namespace Editor {
		void ProjectManagerGui::NewProjectClick() {
			std::string filePath = FileDialog::OpenFolderDialog();
			if (filePath != "") {
				Application->activeProject = new Project();
				Application->activeProject->name;
				Application->activeProject->directory = filePath;
				this->currentContent = new NewProjectContent();
			}
		}
	}
}

