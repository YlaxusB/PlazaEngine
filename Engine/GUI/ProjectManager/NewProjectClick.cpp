#include "Engine/Core/PreCompiledHeaders.h"
#include "ProjectManager.h"

#include "Engine/Application/FileDialog/FileDialog.h"
#include "Engine/Application/Serializer/SceneSerializer.h"
namespace Engine {
	namespace Editor {
		void ProjectManagerGui::NewProjectClick() {
			std::string filePath = FileDialog::SaveFileDialog("");
            std::cout << filePath << std::endl;
			Serializer::Serialize(filePath);
		}
	}
}

