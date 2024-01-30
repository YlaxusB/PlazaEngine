#include "Engine/Core/PreCompiledHeaders.h"
#include "FileSerializer.h"
#include "Engine/Application/Serializer/Components/TextureSerializer.h"
namespace Plaza {
	void MaterialFileSerializer::Serialize(const std::string filePath, Material* material) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		if(!material->name.empty())
		out << YAML::Key << "Material" << YAML::Value << material->name;
		else
			out << YAML::Key << "Material" << YAML::Value << std::filesystem::path{filePath}.stem().string();
		out << YAML::Key << "Uuid" << YAML::Value << material->uuid;
		ComponentSerializer::TextureSerializer::Serialize(out, *material->diffuse);
		ComponentSerializer::TextureSerializer::Serialize(out, *material->specular);
		ComponentSerializer::TextureSerializer::Serialize(out, *material->height);
		ComponentSerializer::TextureSerializer::Serialize(out, *material->normal);
		out << YAML::Key << "Shininess" << YAML::Value << material->shininess;
		out << YAML::EndMap;
		std::ofstream fout(filePath);
		fout << out.c_str();
	}

	Material* MaterialFileSerializer::DeSerialize(const std::string filePath) {
		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());

		Material* material = new Material();
		material->filePath = filePath;
		try {
			if (data) {
				material->name = data["Material"].as<std::string>();
				material->uuid = data["Uuid"].as<uint64_t>();
				if (data["diffuse"]) {
					material->diffuse = ComponentSerializer::TextureSerializer::DeSerialize(data["diffuse"]);
				}
				if (data["specular"]) {
					material->specular = ComponentSerializer::TextureSerializer::DeSerialize(data["specular"]);
				}
				if (data["normal"]) {
					material->normal = ComponentSerializer::TextureSerializer::DeSerialize(data["normal"]);
				}
				material->shininess = data["Shininess"].as<float>();
			}
		}
		catch (exception) {
			std::cout << "Could not load Material: " << filePath << "\n";
		}
		return material;
	}
}