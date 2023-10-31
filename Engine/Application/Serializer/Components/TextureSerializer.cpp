#include "Engine/Core/PreCompiledHeaders.h"
#include "TextureSerializer.h"
namespace Plaza {
	void ComponentSerializer::TextureSerializer::Serialize(YAML::Emitter& out, Texture& texture) {
		if (texture.type != "") {
			out << YAML::Key << texture.type << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "Type" << YAML::Value << texture.type;
			out << YAML::Key << "Path" << YAML::Value << texture.path;
			out << YAML::Key << "Rgba" << YAML::Value << texture.rgba;
			out << YAML::EndMap;
		}
	}

	Texture* ComponentSerializer::TextureSerializer::DeSerialize(YAML::Node data) {
		Texture* texture = new Texture();
		try {
			texture->type = data["Type"].as<std::string>();
			texture->path = data["Path"].as<std::string>();
			texture->rgba = data["Rgba"].as<glm::vec4>();
		}
		catch (exception) {

		}
		return texture;
	}
}