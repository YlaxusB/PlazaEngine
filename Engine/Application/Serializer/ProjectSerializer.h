#pragma once
namespace Plaza {
	class ProjectSerializer {
	public:
		static void Serialize(const std::string filePath);
		static void DeSerialize(const std::string filePath);
	};
}