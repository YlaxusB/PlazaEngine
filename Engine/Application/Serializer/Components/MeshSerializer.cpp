#include "Engine/Core/PreCompiledHeaders.h"
#include "MeshSerializer.h"
#include "ComponentSerializer.h"

#include "Engine/Components/Core/Mesh.h"

// Move the operator<< function definition here
YAML::Emitter& operator <<(YAML::Emitter& out, const Vertex& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.position.x << v.position.y << v.position.z << v.normal.x << v.normal.y << v.normal.z << v.texCoords.x << v.texCoords.y << v.tangent.x << v.tangent.y << v.tangent.z << v.bitangent.x << v.bitangent.y << v.bitangent.z << YAML::EndSeq;
	return out;
}

namespace Engine {
	void MeshSerializer::Serialize(std::string path, Mesh& mesh)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "MeshId" << YAML::Value << mesh.id;
		out << YAML::Key << "Vertices";
		out << YAML::BeginMap;
		for (Vertex vertex : mesh.vertices) {
			out << YAML::Key << "Vertex" << YAML::Value << vertex;
		}
		out << YAML::EndMap;
		out << YAML::Key << "Indices";
		out << YAML::Flow << YAML::BeginSeq;
		for (unsigned int index : mesh.indices) {
			out << index;
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;
		std::ofstream fout(path);
		fout << out.c_str();
	}

	void MeshSerializer::DeSerialize(std::string path)
	{

	}

}