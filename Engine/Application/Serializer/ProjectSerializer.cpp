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
		out << YAML::Key << "Project" << YAML::Value << Application::Get()->activeProject->name;
		out << YAML::Key << "Directory" << YAML::Value << Application::Get()->activeProject->directory;
		out << YAML::Key << "LastActiveScenePath" << YAML::Value << Application::Get()->editorScene->filePath;

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

		Application::Get()->activeProject = new Editor::Project();
		Application::Get()->activeProject->name = name;
		Application::Get()->activeProject->directory = directory;

		if (std::filesystem::exists(Application::Get()->projectPath + "\\" + lastActiveScenePath))
		{
			Serializer::DeSerialize(Application::Get()->projectPath + "\\" + lastActiveScenePath, true);
		}
		else {
			Application::Get()->editorScene = new Scene();
			Application::Get()->activeScene = Application::Get()->editorScene;
			Application::Get()->activeScene->mainSceneEntity = new Entity("Scene");
			Application::Get()->activeScene->mainSceneEntity->parentUuid = Application::Get()->activeScene->mainSceneEntity->uuid;
			Application::Get()->activeScene->GetEntity(Application::Get()->activeScene->mainSceneEntity->uuid)->parentUuid = Application::Get()->activeScene->mainSceneEntity->uuid;
			Editor::DefaultModels::Init();
		}
	}
}