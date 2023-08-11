#include "Engine/Core/PreCompiledHeaders.h"
#include "SceneSerializer.h"

#include "Editor/Settings/EditorSettings.h"

namespace Engine {
	void SerializeGameObject(YAML::Emitter& out, std::unique_ptr<GameObject>& gameObject) {
		out << YAML::BeginMap;
		out << YAML::Key << "GameObject" << YAML::Value << gameObject->uuid;
		out << YAML::Key << "Name" << YAML::Value << gameObject->name;
		out << YAML::Key << "ParentID" << YAML::Value << (!gameObject->parentUuid ? gameObject->GetParent().name : "");
		if (gameObject->GetComponent<Transform>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			glm::vec3& relativePosition = gameObject->GetComponent<Transform>()->relativePosition;
			out << YAML::Key << "Position" << YAML::Value << relativePosition;
			glm::vec3& relativeRotation = gameObject->GetComponent<Transform>()->rotation;
			out << YAML::Key << "Rotation" << YAML::Value << relativeRotation;
			glm::vec3& scale = gameObject->GetComponent<Transform>()->scale;
			out << YAML::Key << "Scale" << YAML::Value << scale;

			out << YAML::EndMap;
		}
		out << YAML::EndMap;
	}

	void Serializer::Serialize(const std::string filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;
		for (std::unique_ptr<GameObject>& gameObject : Application->activeScene->gameObjects) {
			SerializeGameObject(out, gameObject);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	void Serializer::DeSerialize(const std::string filePath)
	{
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Scene"])
			std::cout << "No Scene" << std::endl;

		std::string sceneName = data["Scene"].as<std::string>();

		auto gameObjectsDeserialized = data["GameObjects"];
		if (gameObjectsDeserialized) {
			for (auto gameObject : gameObjectsDeserialized) {
				std::string name = gameObject["Name"].as<std::string>();
				GameObject* newGameObject = new GameObject(name);
				newGameObject->parentUuid = gameObject["ParentID"].as<std::uint64_t>();
				newGameObject->GetComponent<Transform>()->relativePosition = gameObject["TransformComponent"]["Position"].as<glm::vec3>();
				newGameObject->GetComponent<Transform>()->rotation = gameObject["TransformComponent"]["Rotation"].as<glm::vec3>();
				newGameObject->GetComponent<Transform>()->scale = gameObject["TransformComponent"]["Scale"].as<glm::vec3>();

				newGameObject->GetComponent<Transform>()->UpdateChildrenTransform();
				Mesh cubeMesh = Engine::Mesh();
				cubeMesh.material.diffuse.rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);
				cubeMesh.material.specular = Texture();
				cubeMesh.material.specular.rgba = glm::vec4(0.3f, 0.5f, 0.3f, 1.0f);
				MeshRenderer* meshRenderer = new MeshRenderer(cubeMesh);
				//meshRenderer->mesh = cubeMesh;
				//delete meshRenderer;
				newGameObject->AddComponent<MeshRenderer>(meshRenderer);
			}
		}
	}

}