#include "Engine/Core/PreCompiledHeaders.h"
#include "ScriptManagerSerializer.h"
#include "Editor/GUI/Utils/Filesystem.h"
namespace Plaza {
	void ScriptManagerSerializer::Create(std::string fullPath) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Script Manager Path" << YAML::Value << fullPath;
		out << YAML::Key << "Scripts" << YAML::Value << YAML::BeginSeq;
		out << YAML::EndSeq;
		out << YAML::EndMap;
		std::ofstream fout(fullPath);
		fout << out.c_str();
	}

	void ScriptManagerSerializer::AddScript(std::string scriptConfigPath, std::string scriptPath, Script script) {
		std::map<std::string, Script> map = DeSerialize(scriptConfigPath);
		map.emplace(scriptPath, script);
		ScriptManagerSerializer::Serialize(scriptConfigPath, map);
	}

	void ScriptManagerSerializer::Serialize(std::string fullPath, std::map<std::string, Script> scripts) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Script Manager Path" << YAML::Value << fullPath;
		out << YAML::Key << "Scripts" << YAML::Value << YAML::BeginSeq;
		for (auto& [key, value] : scripts) {
			out << YAML::BeginMap;
			out << YAML::Key << "Script" << YAML::Value << key;
			using namespace std::chrono_literals;
			std::chrono::time_point tp = value.lastModifiedDate;
			std::string dateString = std::format("{:%Y%m%d%H%M%S}", tp);
			out << YAML::Key << "Date" << YAML::Value << dateString;
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
		std::ofstream fout(fullPath);
		fout << out.c_str();
	}

	std::map<std::string, Script> ScriptManagerSerializer::DeSerialize(std::string fullPath) {
		std::map<std::string, Script> scriptsMap = std::map<std::string, Script>();
		std::ifstream stream(fullPath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Script Manager Path"])
			std::cout << "No Scripts" << std::endl;
		for (const auto& script : data["Scripts"]) {
			std::string scriptPath = script["Script"].as<string>();

			std::tm timeInfo = {};
			std::istringstream ss(script["Date"].as<string>());

			ss >> std::get_time(&timeInfo, "%Y%m%d%H%M%S");



			std::time_t time = std::mktime(&timeInfo);



			std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(time);


			///std::string scriptDate = script["Date"].as<string>();
			Script script = Script();
			script.lastModifiedDate = tp;

			scriptsMap.emplace(scriptPath, script);
		}
		return scriptsMap;
	}

	/*
	std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			std::cout << "No Scene" << std::endl;

		std::string sceneName = data["Scene"].as<std::string>();

		auto gameObjectsDeserialized = data["GameObjects"];
		if (gameObjectsDeserialized) {
			for (auto entity : gameObjectsDeserialized) {
				std::string name = entity["Name"].as<std::string>();
				Entity* newGameObject = new Entity(name);
				newGameObject->parentUuid = entity["ParentID"].as<std::uint64_t>();
				newGameObject->GetComponent<Transform>()->relativePosition = entity["TransformComponent"]["Position"].as<glm::vec3>();
				newGameObject->GetComponent<Transform>()->rotation = entity["TransformComponent"]["Rotation"].as<glm::vec3>();
				newGameObject->GetComponent<Transform>()->scale = entity["TransformComponent"]["Scale"].as<glm::vec3>();

				newGameObject->GetComponent<Transform>()->UpdateChildrenTransform();
				Mesh cubeMesh = Plaza::Mesh();
				cubeMesh.material.diffuse.rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);
				cubeMesh.material.specular = Texture();
				cubeMesh.material.specular.rgba = glm::vec4(0.3f, 0.5f, 0.3f, 1.0f);
				MeshRenderer* meshRenderer = new MeshRenderer(cubeMesh);
				//meshRenderer->mesh = cubeMesh;
				//delete meshRenderer;
				newGameObject->AddComponent<MeshRenderer>(meshRenderer);
			}
		}
	*/
}