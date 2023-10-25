#include "Engine/Core/PreCompiledHeaders.h"
#include "MeshRendererSerializer.h"
#include "MeshSerializer.h"
namespace Plaza {
	void ComponentSerializer::MeshRendererSerializer::Serialize(YAML::Emitter& out, MeshRenderer& meshRenderer) {
		out << YAML::Key << "MeshRendererComponent" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Uuid" << YAML::Value << meshRenderer.uuid;
		out << YAML::Key << "CastShadows" << YAML::Value << meshRenderer.castShadows;
		out << YAML::Key << "AiMeshName" << YAML::Value << meshRenderer.aiMeshName;
		out << YAML::Key << "MeshName" << YAML::Value << meshRenderer.meshName;
		out << YAML::Key << "Instanced" << YAML::Value << meshRenderer.instanced;
		out << YAML::Key << "MeshId" << YAML::Value << meshRenderer.mesh->meshId;
		//ComponentSerializer::MeshSerializer::Serialize(out, *meshRenderer.mesh);
		out << YAML::EndMap;
	}
	void ComponentSerializer::MeshRendererSerializer::DeSerialize(YAML::Emitter& out, MeshRenderer& meshRenderer)
	{
		std::cout << "hey" << std::endl;
	}
}