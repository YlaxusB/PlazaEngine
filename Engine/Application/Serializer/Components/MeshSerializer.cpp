#include "Engine/Core/PreCompiledHeaders.h"
#include "MeshSerializer.h"
//#include "ComponentSerializer.h"

#include "Engine/Components/Rendering/Mesh.h"

// Move the operator<< function definition here
YAML::Emitter& operator <<(YAML::Emitter& out, const Vertex& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.position.x << v.position.y << v.position.z << v.normal.x << v.normal.y << v.normal.z << v.texCoords.x << v.texCoords.y << v.tangent.x << v.tangent.y << v.tangent.z << v.bitangent.x << v.bitangent.y << v.bitangent.z << YAML::EndSeq;
	return out;
}

namespace Plaza {
	void ComponentSerializer::MeshSerializer::Serialize(YAML::Emitter& out, Mesh& mesh)
	{
		out << YAML::Value << "MeshComponent" << YAML::BeginMap;
		out << YAML::Key << "MeshId" << YAML::Value << mesh.meshId;
		out << YAML::EndMap;
	}

	void ComponentSerializer::MeshSerializer::DeSerialize(std::string path)
	{

	}

}