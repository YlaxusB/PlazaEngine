#pragma once
namespace Engine {
	class MeshSerializer {
	public:
		static void Serialize(std::string path, Mesh& mesh);
		static void DeSerialize(std::string path);
	};
}