#pragma once
namespace Plaza {
	class Serializer {
	public:
		static void Serialize(const Asset* sceneAsset);
		static void DeSerialize(const std::string filePath, bool deserializingProject = false);
	};
}