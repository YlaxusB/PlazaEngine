#include "Engine/Core/PreCompiledHeaders.h"
#include "MaterialSerializer.h"
#include "TextureSerializer.h"
namespace Plaza {
	void ComponentSerializer::MaterialSerializer::Serialize(YAML::Emitter& out, Material& material) {
		out << YAML::Key << "MaterialComponent" << YAML::Value << YAML::BeginMap;
		TextureSerializer::Serialize(out, *material.diffuse);
		TextureSerializer::Serialize(out, *material.specular);
		TextureSerializer::Serialize(out, *material.height);
		TextureSerializer::Serialize(out, *material.normal);
		out << YAML::Key << "Shininess" << YAML::Value << material.shininess;
		out << YAML::EndMap;
	}
	void ComponentSerializer::MaterialSerializer::DeSerialize(YAML::Emitter& out, Material& material)
	{
	}
}