#pragma once
namespace Plaza {
	class Serializer {
	public:
		static void Serialize(const Asset* asset);
		static void DeSerialize(const std::string filePath, bool deserializingProject = false);
	};
}