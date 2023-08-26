#include "Engine/Core/PreCompiledHeaders.h"
#include "TextureSerializer.h"
namespace Plaza {
	void ComponentSerializer::TextureSerializer::Serialize(YAML::Emitter& out, Texture& texture) {
		if (texture.path != "" || texture.rgba != glm::vec4(INFINITY)) {
			out << YAML::Key << texture.type << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "Path" << YAML::Value << texture.path;
			out << YAML::Key << "Rgba" << YAML::Value << texture.rgba;
			out << YAML::EndMap;
		}
	}
}