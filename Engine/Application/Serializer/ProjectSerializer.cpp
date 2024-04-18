#include "Engine/Core/PreCompiledHeaders.h"
#include "ProjectSerializer.h"
#include "Engine/Utils/yamlUtils.h"

#include "SceneSerializer.h"
#include "Editor/DefaultAssets/Models/DefaultModels.h"
namespace Plaza {
	void ProjectSerializer::Serialize(const std::string filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value << Application->activeProject->name;
		out << YAML::Key << "Directory" << YAML::Value << Application->activeProject->directory;
		out << YAML::Key << "LastActiveScenePath" << YAML::Value << Application->editorScene->filePath;

		//out << YAML::EndSeq;
		out << YAML::EndMap;
		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void ProjectSerializer::DeSerialize(const std::string filePath)
	{
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data) {
			std::cout << "File is empty!" << std::endl;
			return;
		}

		std::string name = data["Project"].as<std::string>();
		std::string directory = data["Directory"].as<std::string>();
		std::string lastActiveScenePath = data["LastActiveScenePath"].as<std::string>();

		Application->activeProject = new Editor::Project();
		Application->activeProject->name = name;
		Application->activeProject->directory = directory;

		if (std::filesystem::exists(Application->projectPath + "\\" + lastActiveScenePath))
		{
			Serializer::DeSerialize(Application->projectPath + "\\" + lastActiveScenePath, true);
		}
		else {
			Application->editorScene = new Scene();
			Application->activeScene = Application->editorScene;
			Application->activeScene->mainSceneEntity = new Entity("Scene");
			Application->activeScene->mainSceneEntity->parentUuid = Application->activeScene->mainSceneEntity->uuid;
			Application->activeScene->GetEntity(Application->activeScene->mainSceneEntity->uuid)->parentUuid = Application->activeScene->mainSceneEntity->uuid;
			Editor::DefaultModels::Init();
		}
	}
}