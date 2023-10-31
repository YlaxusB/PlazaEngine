#pragma once
namespace Plaza {
	class Serializer {
	public:
		static void Serialize(const std::string filePath);
		static void DeSerialize(const std::string filePath, bool deserializingProject = false);
	};
}