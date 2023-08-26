#include "Engine/Core/PreCompiledHeaders.h"
#include "SceneSerializer.h"

#include "Editor/Settings/EditorSettings.h"

#include "Engine/Application/Serializer/Components/TransformSerializer.h"
#include "Engine/Application/Serializer/Components/MeshRendererSerializer.h"
namespace Plaza {
	void SerializeGameObjectd(YAML::Emitter& out, Entity* entity) {
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity->uuid;
		out << YAML::Key << "Name" << YAML::Value << entity->name;
		out << YAML::Key << "ParentID" << YAML::Value << (!entity->parentUuid ? entity->GetParent().name : "");
		out << YAML::Key << "Components" << YAML::BeginMap;
		if (Transform* transform = entity->GetComponent<Transform>()) {
			ComponentSerializer::TransformSerializer::Serialize(out, *transform);
		}
		if (MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>()) {
			ComponentSerializer::MeshRendererSerializer::Serialize(out, *meshRenderer);
		}
		out << YAML::EndMap;

		out << YAML::EndMap;
	}

	void Serializer::Serialize(const std::string filePath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "GameObjects" << YAML::Value << YAML::BeginSeq;
		for (auto& [key, value] : Application->activeScene->entities) {
			SerializeGameObjectd(out, &value);
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
	}

}